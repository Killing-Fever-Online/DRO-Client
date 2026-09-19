#include "draudiostream.h"

#include "dro/system/audio/audio_backend.h"
#include "dro/system/audio/miniaudio_config.h"
#include "dro/system/audio/network_stream.h"

#include <QDebug>
#include <QTimer>
#include <QUrl>

#include <cmath>

#include "draudioengine.h"

class DRAudioStreamPrivate
{
public:
  ma_decoder decoder;
  ma_sound sound;
  ma_node *reverb_node = nullptr;
  ma_node *pitch_tempo_node = nullptr;
  bool decoder_ready = false;
  bool sound_ready = false;
  NetworkStream *network = nullptr;
};

namespace
{
void sound_end_proc(void *pUserData, ma_sound *pSound)
{
  Q_UNUSED(pSound);
  DRAudioStream *l_stream = static_cast<DRAudioStream *>(pUserData);
  QMetaObject::invokeMethod(l_stream, &DRAudioStream::handle_end, Qt::QueuedConnection);
}

float semitones_to_ratio(float p_semitones)
{
  return std::pow(2.0f, p_semitones / 12.0f);
}

float percent_to_ratio(float p_percent)
{
  const float l_ratio = 1.0f + (p_percent * 0.01f);
  return l_ratio <= 0.0f ? 0.01f : l_ratio;
}
} // namespace

DRAudioStream::DRAudioStream(DRAudio::Family p_family)
    : m_engine(new DRAudioEngine(this))
    , m_family(p_family)
    , m_fade(NoFade)
    , d(new DRAudioStreamPrivate)
{
  registerMetatypes();

  m_fade_timer = new QTimer(this);
  m_fade_timer->setSingleShot(true);
  connect(m_fade_timer, &QTimer::timeout, this, &DRAudioStream::handle_fade_finished);

  m_drain_timer = new QTimer(this);
  m_drain_timer->setSingleShot(false);
  m_drain_timer->setInterval(25);
  connect(m_drain_timer, &QTimer::timeout, this, &DRAudioStream::check_drain);

  connect(m_engine, &DRAudioEngine::current_device_changed, this, &DRAudioStream::update_device);
}

void DRAudioStream::registerMetatypes()
{
  qRegisterMetaType<DRAudioStream::Fade>();
}

DRAudioStream::~DRAudioStream()
{
  release_sound();
}

void DRAudioStream::release_sound()
{
  if (d->sound_ready)
  {
    ma_sound_uninit(&d->sound);
    d->sound_ready = false;
  }
  if (d->reverb_node != nullptr)
  {
    audio_backend::reverb_node_uninit(d->reverb_node);
    d->reverb_node = nullptr;
  }
  if (d->pitch_tempo_node != nullptr)
  {
    audio_backend::soundtouch_node_uninit(d->pitch_tempo_node);
    d->pitch_tempo_node = nullptr;
  }
  if (d->decoder_ready)
  {
    ma_decoder_uninit(&d->decoder);
    d->decoder_ready = false;
  }
  if (d->network != nullptr)
  {
    delete d->network;
    d->network = nullptr;
  }
}

DRAudio::Family DRAudioStream::get_family() const
{
  return m_family;
}

QString DRAudioStream::get_file_name() const
{
  return m_filename;
}

DRAudioStream::Fade DRAudioStream::get_fade() const
{
  return m_fade;
}

bool DRAudioStream::is_playing() const
{
  if (!ensure_init())
    return false;
  return ma_sound_is_playing(&d->sound) != 0;
}

bool DRAudioStream::is_repeatable() const
{
  return m_repeatable;
}

void DRAudioStream::play()
{
  // handlers may drop our last reference, keep us alive until we return
  const ptr l_self = sharedFromThis();

  if (!ensure_init())
    return;
  const ma_result l_result = ma_sound_start(&d->sound);
  if (l_result != MA_SUCCESS)
  {
    qWarning() << QString("error: failed to play file: %1 (file: \"%2\")")
                      .arg(audio_backend::error_string(l_result), m_filename);
    Q_EMIT finished();
  }
}

void DRAudioStream::playSynced(const DRAudioStream *reference)
{
  // handlers may drop our last reference, keep us alive until we return
  const ptr l_self = sharedFromThis();

  if (!ensure_init() || !reference || !reference->ensure_init())
  {
    play();
    return;
  }

  ma_uint64 l_cursor = 0;
  if (ma_sound_get_cursor_in_pcm_frames(&reference->d->sound, &l_cursor) == MA_SUCCESS)
  {
    ma_sound_seek_to_pcm_frame(&d->sound, l_cursor);
  }

  const ma_result l_result = ma_sound_start(&d->sound);
  if (l_result != MA_SUCCESS)
  {
    qWarning() << QString("error: failed to play synced stream: %1 (file: \"%2\")")
                      .arg(audio_backend::error_string(l_result), m_filename);
    Q_EMIT finished();
  }
}

void DRAudioStream::stop()
{
  // handlers may drop our last reference, keep us alive until we return
  const ptr l_self = sharedFromThis();

  m_drain_timer->stop();
  if (!ensure_init())
    return;
  ma_sound_stop(&d->sound);
  Q_EMIT finished();
}

void DRAudioStream::handle_end()
{
  // handlers may drop our last reference, keep us alive until we return
  const ptr l_self = sharedFromThis();

  if (!m_repeatable)
  {
    if (d->pitch_tempo_node != nullptr && !audio_backend::soundtouch_node_is_drained(d->pitch_tempo_node))
    {
      m_drain_ticks = 0;
      m_drain_timer->start();
      return;
    }
    m_drain_timer->stop();
    Q_EMIT finished();
  }
  else
  {
    Q_EMIT looped();
  }
}

std::optional<DRAudioError> DRAudioStream::set_file_name(QString p_file_name)
{
  m_url.clear();
  m_filename = p_file_name;
  release_sound();
  m_init_state = InitNotDone;
  if (!ensure_init())
  {
    return DRAudioError("failed to set file: " + p_file_name);
  }
  Q_EMIT file_name_changed(m_filename);
  return std::nullopt;
}

std::optional<DRAudioError> DRAudioStream::SetWebAddress(QString t_url)
{
  m_url = t_url;
  m_filename.clear();
  release_sound();
  m_init_state = InitNotDone;
  if (!ensure_init())
  {
    return DRAudioError("failed to set url: " + t_url);
  }
  Q_EMIT file_name_changed(m_filename);
  return std::nullopt;
}

void DRAudioStream::set_pitch(float pitch)
{
  m_pitch = pitch;
  if (!ensure_init())
    return;
  apply_rate();
}

void DRAudioStream::set_speed(float speed)
{
  m_speed = speed;
  if (!ensure_init())
    return;
  apply_rate();
}

void DRAudioStream::set_family_pitch(float pitch)
{
  m_family_pitch = pitch;
  if (!ensure_init())
    return;
  apply_rate();
}

void DRAudioStream::refresh_rate_mode()
{
  if (!ensure_init())
    return;
  rewire();
}

void DRAudioStream::set_family_speed(float speed)
{
  m_family_speed = speed;
  if (!ensure_init())
    return;
  apply_rate();
}

void DRAudioStream::apply_rate()
{
  if (!d->sound_ready)
    return;

  const float l_pitch = DRAudioEngine::get_pitch() + m_family_pitch + m_pitch;
  const float l_speed = DRAudioEngine::get_speed() + m_family_speed + m_speed;

  if (DRAudioEngine::is_option(DRAudio::OEngineIndependentPitchTempo))
  {
    if (d->pitch_tempo_node != nullptr)
    {
      audio_backend::soundtouch_node_set_pitch(d->pitch_tempo_node, l_pitch);
      audio_backend::soundtouch_node_set_tempo(d->pitch_tempo_node, percent_to_ratio(l_speed));
    }
    ma_sound_set_pitch(&d->sound, 1.0f);
  }
  else
  {
    ma_sound_set_pitch(&d->sound, semitones_to_ratio(l_pitch) * percent_to_ratio(l_speed));
  }
}

void DRAudioStream::rewire()
{
  if (!d->sound_ready)
    return;

  ma_engine *l_engine = audio_backend::engine();
  if (l_engine == nullptr)
    return;

  ma_node_graph *l_graph = ma_engine_get_node_graph(l_engine);
  ma_node *l_endpoint = ma_node_graph_get_endpoint(l_graph);
  const bool l_independent = DRAudioEngine::is_option(DRAudio::OEngineIndependentPitchTempo);

  if (l_independent && d->pitch_tempo_node == nullptr)
  {
    const ma_uint32 l_channels = ma_engine_get_channels(l_engine);
    const ma_uint32 l_sample_rate = ma_engine_get_sample_rate(l_engine);
    if (audio_backend::soundtouch_node_init(l_graph, l_channels, l_sample_rate, &d->pitch_tempo_node) != MA_SUCCESS)
    {
      d->pitch_tempo_node = nullptr;
    }
  }
  else if (!l_independent && d->pitch_tempo_node != nullptr)
  {
    audio_backend::soundtouch_node_uninit(d->pitch_tempo_node);
    d->pitch_tempo_node = nullptr;
  }

  if (m_reverb && d->reverb_node == nullptr)
  {
    const ma_uint32 l_channels = ma_engine_get_channels(l_engine);
    const ma_uint32 l_sample_rate = ma_engine_get_sample_rate(l_engine);
    if (audio_backend::reverb_node_init(l_graph, l_channels, l_sample_rate, &d->reverb_node) != MA_SUCCESS)
    {
      d->reverb_node = nullptr;
    }
  }
  else if (!m_reverb && d->reverb_node != nullptr)
  {
    audio_backend::reverb_node_uninit(d->reverb_node);
    d->reverb_node = nullptr;
  }

  ma_node *l_prev = reinterpret_cast<ma_node *>(&d->sound);
  if (l_independent && d->pitch_tempo_node != nullptr)
  {
    ma_node_attach_output_bus(l_prev, 0, d->pitch_tempo_node, 0);
    l_prev = d->pitch_tempo_node;
  }
  if (m_reverb && d->reverb_node != nullptr)
  {
    ma_node_attach_output_bus(l_prev, 0, d->reverb_node, 0);
    l_prev = d->reverb_node;
  }
  ma_node_attach_output_bus(l_prev, 0, l_endpoint, 0);

  apply_rate();
}

void DRAudioStream::set_volume(float p_volume)
{
  m_volume = p_volume;
  if (!ensure_init())
    return;
  update_volume();
}

void DRAudioStream::set_repeatable(bool p_enabled)
{
  if (m_repeatable == p_enabled)
    return;
  m_repeatable = p_enabled;
  init_loop();
}

void DRAudioStream::set_loop(quint64 p_start, quint64 p_end)
{
  if (m_loop_start == p_start && m_loop_end == p_end)
    return;
  m_loop_start = p_start;
  m_loop_end = p_end;
  init_loop();
}

void DRAudioStream::fade(Fade p_fade, int p_duration)
{
  if (p_fade == NoFade || p_fade == m_fade || !ensure_init())
  {
    return;
  }

  // the fader multiplies on top of the sound volume, so it ramps 0..1
  const float l_begin = (p_fade == FadeOut) ? 1.0f : 0.0f;
  const float l_end = (p_fade == FadeOut) ? 0.0f : 1.0f;

  m_fade = p_fade;
  m_fade_duration = qMax(0, p_duration);
  m_fade_running = true;

  ma_sound_set_fade_in_milliseconds(&d->sound, l_begin, l_end, ma_uint64(m_fade_duration));

  m_fade_timer->start(m_fade_duration);
}

void DRAudioStream::fadeIn(int p_duration)
{
  fade(FadeIn, p_duration);
}

void DRAudioStream::fadeOut(int p_duration)
{
  fade(FadeOut, p_duration);
}

void DRAudioStream::handle_fade_finished()
{
  // handlers may drop our last reference, keep us alive until we return
  const ptr l_self = sharedFromThis();

  m_fade_running = false;

  if (m_fade == FadeOut)
  {
    stop();
  }
}

bool DRAudioStream::ensure_init()
{
  if (m_init_state != InitNotDone)
    return m_init_state == InitFinished;
  m_init_state = InitError;

  ma_engine *l_engine = audio_backend::engine();
  if (l_engine == nullptr)
  {
    return false;
  }

  ma_decoder_config l_config = ma_decoder_config_init(ma_format_f32, 0, 0);
  l_config.ppCustomBackendVTables = audio_backend::custom_backends();
  l_config.customBackendCount = audio_backend::custom_backend_count();

  ma_result l_result = MA_ERROR;
  if (!m_url.isEmpty())
  {
    d->network = new NetworkStream(QUrl(m_url));
    if (!d->network->wait_for_header())
    {
      qWarning() << "error: failed to open URL for streaming:" << m_url;
      delete d->network;
      d->network = nullptr;
      return false;
    }

    l_result = audio_backend::decoder_init_with_tell(&NetworkStream::on_read, &NetworkStream::on_seek, &NetworkStream::on_tell, d->network, &l_config, &d->decoder);
  }
  else if (m_filename.isEmpty())
  {
    return false;
  }
  else
  {
    l_result = ma_decoder_init_file_w(reinterpret_cast<const wchar_t *>(m_filename.utf16()), &l_config, &d->decoder);
  }

  if (l_result != MA_SUCCESS)
  {
    qWarning() << "error: failed to create decoder:" << audio_backend::error_string(l_result);
    if (d->network != nullptr)
    {
      delete d->network;
      d->network = nullptr;
    }
    return false;
  }
  d->decoder_ready = true;

  l_result = ma_sound_init_from_data_source(l_engine, &d->decoder, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &d->sound);
  if (l_result != MA_SUCCESS)
  {
    qWarning() << "error: failed to create sound:" << audio_backend::error_string(l_result);
    ma_decoder_uninit(&d->decoder);
    d->decoder_ready = false;
    return false;
  }
  d->sound_ready = true;

  ma_sound_set_end_callback(&d->sound, sound_end_proc, this);

  m_init_state = InitFinished;
  init_loop();
  rewire();
  update_volume();
  return true;
}

bool DRAudioStream::ensure_init() const
{
  return const_cast<DRAudioStream *>(this)->ensure_init();
}

void DRAudioStream::init_loop()
{
  if (!d->decoder_ready)
    return;

  ma_data_source *l_source = &d->decoder;
  ma_data_source_set_looping(l_source, m_repeatable ? MA_TRUE : MA_FALSE);

  if (!m_repeatable)
  {
    return;
  }

  ma_uint64 l_begin = m_loop_start;
  ma_uint64 l_end = m_loop_end;

  if (m_url.isEmpty())
  {
    ma_uint64 l_length = 0;
    ma_data_source_get_length_in_pcm_frames(l_source, &l_length);

    if (l_length > 0)
    {
      if (l_begin >= l_length)
      {
        l_begin = 0;
      }
      if (l_end <= l_begin || l_end > l_length)
      {
        l_end = l_length;
      }
    }
    else
    {
      l_end = ~((ma_uint64)0);
    }
  }
  else
  {
    l_begin = 0;
    l_end = ~((ma_uint64)0);
  }

  ma_data_source_set_loop_point_in_pcm_frames(l_source, l_begin, l_end);
}

void DRAudioStream::toggle_reverb(bool reverb)
{
  m_reverb = reverb;
  if (!d->sound_ready)
    return;
  rewire();
}

void DRAudioStream::update_device(DRAudioDevice p_device)
{
  Q_UNUSED(p_device);

  if (m_init_state != InitFinished)
    return;

  ma_uint64 l_cursor = 0;
  const bool l_was_playing = is_playing();
  if (d->sound_ready)
  {
    ma_sound_get_cursor_in_pcm_frames(&d->sound, &l_cursor);
  }

  release_sound();
  m_init_state = InitNotDone;

  if (!ensure_init())
  {
    return;
  }

  if (l_cursor > 0)
  {
    ma_sound_seek_to_pcm_frame(&d->sound, l_cursor);
  }
  if (l_was_playing)
  {
    ma_sound_start(&d->sound);
  }
}

void DRAudioStream::update_volume()
{
  if (!d->sound_ready)
    return;
  ma_sound_set_volume(&d->sound, m_volume * 0.01f);
}

void DRAudioStream::check_drain()
{
  // the drained flag may never latch, so cap the wait
  m_drain_ticks += 1;
  if (m_drain_ticks < 20 && d->pitch_tempo_node != nullptr && !audio_backend::soundtouch_node_is_drained(d->pitch_tempo_node))
  {
    return;
  }
  // handlers may drop our last reference, keep us alive until we return
  const ptr l_self = sharedFromThis();
  m_drain_timer->stop();
  Q_EMIT finished();
}
