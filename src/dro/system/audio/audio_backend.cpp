#include "audio_backend.h"

#include "dro/system/audio/miniaudio_config.h"

#include <QDebug>
#include <cstring>

namespace
{
struct BackendState
{
  ma_context context;
  ma_engine engine;
  bool context_ready = false;
  bool engine_ready = false;
  QByteArray device_id;
};

BackendState &state()
{
  static BackendState s_state;
  return s_state;
}

bool ensure_context()
{
  BackendState &l_state = state();
  if (l_state.context_ready)
  {
    return true;
  }

  const ma_result l_result = ma_context_init(nullptr, 0, nullptr, &l_state.context);
  if (l_result != MA_SUCCESS)
  {
    qWarning() << "error: failed to initialize audio context:" << ma_result_description(l_result);
    return false;
  }

  l_state.context_ready = true;
  return true;
}

QByteArray encode_device_id(const ma_device_id &p_id)
{
  return QByteArray(reinterpret_cast<const char *>(&p_id), sizeof(ma_device_id));
}

bool decode_device_id(const QByteArray &p_data, ma_device_id &r_id)
{
  if (p_data.size() != sizeof(ma_device_id))
  {
    return false;
  }
  memcpy(&r_id, p_data.constData(), sizeof(ma_device_id));
  return true;
}

void destroy_engine()
{
  BackendState &l_state = state();
  if (!l_state.engine_ready)
  {
    return;
  }
  ma_engine_uninit(&l_state.engine);
  l_state.engine_ready = false;
}
} // namespace

QVector<audio_backend::DeviceEntry> audio_backend::enumerate_devices()
{
  QVector<DeviceEntry> r_list;
  if (!ensure_context())
  {
    return r_list;
  }

  ma_device_info *l_playback_infos = nullptr;
  ma_uint32 l_playback_count = 0;
  const ma_result l_result =
      ma_context_get_devices(&state().context, &l_playback_infos, &l_playback_count, nullptr, nullptr);
  if (l_result != MA_SUCCESS)
  {
    qWarning() << "error: failed to enumerate audio devices:" << ma_result_description(l_result);
    return r_list;
  }

  for (ma_uint32 i = 0; i < l_playback_count; ++i)
  {
    DeviceEntry l_entry;
    l_entry.index = i;
    l_entry.name = QString::fromUtf8(l_playback_infos[i].name);
    l_entry.native_id = encode_device_id(l_playback_infos[i].id);
    l_entry.driver = QString::fromLatin1(l_entry.native_id.toHex());
    l_entry.is_default = l_playback_infos[i].isDefault != 0;
    r_list.append(l_entry);
  }

  return r_list;
}

bool audio_backend::select_device(const QByteArray &p_native_id)
{
  if (!ensure_context())
  {
    return false;
  }

  BackendState &l_state = state();
  destroy_engine();

  ma_device_id l_device_id;
  const bool l_has_id = decode_device_id(p_native_id, l_device_id);

  ma_engine_config l_config = ma_engine_config_init();
  l_config.pContext = &l_state.context;
  if (l_has_id)
  {
    l_config.pPlaybackDeviceID = &l_device_id;
  }

  const ma_result l_result = ma_engine_init(&l_config, &l_state.engine);
  if (l_result != MA_SUCCESS)
  {
    qWarning() << "error: failed to initialize audio engine:" << ma_result_description(l_result);
    return false;
  }

  l_state.engine_ready = true;
  l_state.device_id = p_native_id;
  return true;
}

QByteArray audio_backend::selected_device()
{
  return state().device_id;
}

bool audio_backend::is_started()
{
  return state().engine_ready;
}

ma_engine *audio_backend::engine()
{
  BackendState &l_state = state();
  if (!l_state.engine_ready)
  {
    return nullptr;
  }
  return &l_state.engine;
}

void audio_backend::shutdown()
{
  BackendState &l_state = state();
  destroy_engine();
  if (l_state.context_ready)
  {
    ma_context_uninit(&l_state.context);
    l_state.context_ready = false;
  }
  l_state.device_id.clear();
}

QString audio_backend::error_string(int32_t p_result)
{
  return QString::fromUtf8(ma_result_description(static_cast<ma_result>(p_result)));
}
