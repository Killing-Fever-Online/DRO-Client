#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio_config.h"

#include <miniaudio/extras/decoders/libvorbis/miniaudio_libvorbis.h>
#include <miniaudio/extras/decoders/libopus/miniaudio_libopus.h>

#include <atomic>
#include <vector>

#include <SoundTouch.h>

namespace
{
ma_decoding_backend_vtable *g_backends[] = {
    ma_decoding_backend_libvorbis,
    ma_decoding_backend_libopus,
};

struct CombFilter
{
  std::vector<float> buffer;
  size_t index = 0;
  float store = 0.0f;
  float feedback = 0.0f;
  float damp = 0.0f;

  void resize(size_t p_size)
  {
    buffer.assign(p_size, 0.0f);
    index = 0;
    store = 0.0f;
  }

  float process(float p_input)
  {
    const float l_output = buffer[index];
    store = l_output * (1.0f - damp) + store * damp;
    buffer[index] = p_input + store * feedback;
    if (++index >= buffer.size())
    {
      index = 0;
    }
    return l_output;
  }
};

struct AllpassFilter
{
  std::vector<float> buffer;
  size_t index = 0;
  float feedback = 0.5f;

  void resize(size_t p_size)
  {
    buffer.assign(p_size, 0.0f);
    index = 0;
  }

  float process(float p_input)
  {
    const float l_buffered = buffer[index];
    const float l_output = -p_input + l_buffered;
    buffer[index] = p_input + l_buffered * feedback;
    if (++index >= buffer.size())
    {
      index = 0;
    }
    return l_output;
  }
};

struct ReverbChannel
{
  CombFilter combs[4];
  AllpassFilter allpasses[2];
};

struct ReverbNode
{
  ma_node_base base;
  ma_uint32 channels = 0;
  std::vector<ReverbChannel> channel_state;
  float wet = 0.28f;
  float dry = 0.72f;
};

const size_t COMB_TUNING[4] = {1116, 1188, 1277, 1356};
const size_t ALLPASS_TUNING[2] = {556, 441};

void reverb_process(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut,
                    ma_uint32 *pFrameCountOut)
{
  ReverbNode *l_node = reinterpret_cast<ReverbNode *>(pNode);
  const ma_uint32 l_frame_count = *pFrameCountOut < *pFrameCountIn ? *pFrameCountOut : *pFrameCountIn;
  const ma_uint32 l_channels = l_node->channels;

  const float *l_in = ppFramesIn[0];
  float *l_out = ppFramesOut[0];

  for (ma_uint32 iFrame = 0; iFrame < l_frame_count; ++iFrame)
  {
    for (ma_uint32 iChannel = 0; iChannel < l_channels; ++iChannel)
    {
      const float l_input = l_in[iFrame * l_channels + iChannel];
      ReverbChannel &l_state = l_node->channel_state[iChannel];

      float l_sum = 0.0f;
      for (int i = 0; i < 4; ++i)
      {
        l_sum += l_state.combs[i].process(l_input * 0.015f);
      }
      for (int i = 0; i < 2; ++i)
      {
        l_sum = l_state.allpasses[i].process(l_sum);
      }

      l_out[iFrame * l_channels + iChannel] = l_input * l_node->dry + l_sum * l_node->wet;
    }
  }

  *pFrameCountIn = l_frame_count;
  *pFrameCountOut = l_frame_count;
}

ma_node_vtable g_reverb_vtable = {
    reverb_process,
    nullptr,
    1,
    1,
    0,
};
} // namespace

ma_decoding_backend_vtable **audio_backend::custom_backends()
{
  return g_backends;
}

ma_uint32 audio_backend::custom_backend_count()
{
  return sizeof(g_backends) / sizeof(g_backends[0]);
}

ma_result audio_backend::decoder_init_with_tell(ma_decoder_read_proc p_on_read, ma_decoder_seek_proc p_on_seek,
                                                ma_decoder_tell_proc p_on_tell, void *p_user_data,
                                                const ma_decoder_config *p_config, ma_decoder *p_decoder)
{
  ma_decoder_config l_config = ma_decoder_config_init_copy(p_config);
  ma_result l_result = ma_decoder__preinit(p_on_read, p_on_seek, p_on_tell, p_user_data, &l_config, p_decoder);
  if (l_result != MA_SUCCESS)
  {
    return l_result;
  }
  return ma_decoder_init__internal(p_on_read, p_on_seek, p_user_data, &l_config, p_decoder);
}

ma_result audio_backend::reverb_node_init(ma_node_graph *p_graph, ma_uint32 p_channels, ma_uint32 p_sample_rate,
                                          ma_node **p_node)
{
  if (p_graph == nullptr || p_node == nullptr || p_channels == 0)
  {
    return MA_INVALID_ARGS;
  }

  ReverbNode *l_node = new (std::nothrow) ReverbNode();
  if (l_node == nullptr)
  {
    return MA_OUT_OF_MEMORY;
  }

  l_node->channels = p_channels;
  l_node->channel_state.resize(p_channels);

  const double l_scale = double(p_sample_rate) / 44100.0;
  for (ma_uint32 iChannel = 0; iChannel < p_channels; ++iChannel)
  {
    ReverbChannel &l_state = l_node->channel_state[iChannel];
    const size_t l_spread = size_t(iChannel) * 23;
    for (int i = 0; i < 4; ++i)
    {
      l_state.combs[i].resize(size_t(double(COMB_TUNING[i] + l_spread) * l_scale) + 1);
      l_state.combs[i].feedback = 0.84f;
      l_state.combs[i].damp = 0.2f;
    }
    for (int i = 0; i < 2; ++i)
    {
      l_state.allpasses[i].resize(size_t(double(ALLPASS_TUNING[i] + l_spread) * l_scale) + 1);
      l_state.allpasses[i].feedback = 0.5f;
    }
  }

  ma_node_config l_config = ma_node_config_init();
  l_config.vtable = &g_reverb_vtable;
  l_config.pInputChannels = &l_node->channels;
  l_config.pOutputChannels = &l_node->channels;

  const ma_result l_result = ma_node_init(p_graph, &l_config, nullptr, &l_node->base);
  if (l_result != MA_SUCCESS)
  {
    delete l_node;
    return l_result;
  }

  *p_node = reinterpret_cast<ma_node *>(l_node);
  return MA_SUCCESS;
}

void audio_backend::reverb_node_uninit(ma_node *p_node)
{
  if (p_node == nullptr)
  {
    return;
  }
  ReverbNode *l_node = reinterpret_cast<ReverbNode *>(p_node);
  ma_node_uninit(&l_node->base, nullptr);
  delete l_node;
}

namespace
{
struct SoundTouchNode
{
  ma_node_base base;
  ma_uint32 channels = 0;
  soundtouch::SoundTouch processor;
  std::atomic<float> pitch_semitones{0.0f};
  std::atomic<float> tempo{1.0f};
  std::atomic<ma_uint32> remaining_frames{0};
  std::atomic<bool> flushed{false};
};

void soundtouch_process(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut,
                        ma_uint32 *pFrameCountOut)
{
  SoundTouchNode *l_node = reinterpret_cast<SoundTouchNode *>(pNode);
  const ma_uint32 l_cap_frames = *pFrameCountOut;
  const ma_uint32 l_frames_in = *pFrameCountIn;

  l_node->processor.setTempo(l_node->tempo.load());
  l_node->processor.setPitchSemiTones(static_cast<double>(l_node->pitch_semitones.load()));

  if (l_frames_in > 0)
  {
    l_node->processor.putSamples(ppFramesIn[0], l_frames_in);
  }
  else if (!l_node->flushed.load())
  {
    l_node->processor.flush();
    l_node->flushed.store(true);
  }

  const ma_uint32 l_avail = l_node->processor.numSamples();
  const ma_uint32 l_to_receive = l_avail < l_cap_frames ? l_avail : l_cap_frames;

  if (l_to_receive > 0)
  {
    const ma_uint32 l_got = l_node->processor.receiveSamples(ppFramesOut[0], l_to_receive);
    *pFrameCountOut = l_got;
    l_node->remaining_frames.store(l_avail - l_got);
  }
  else
  {
    *pFrameCountOut = 0;
    l_node->remaining_frames.store(l_avail);
  }

  *pFrameCountIn = l_frames_in;
}

ma_result soundtouch_get_required_input_frames(ma_node *pNode, ma_uint32 outputFrameCount, ma_uint32 *pInputFrameCount)
{
  SoundTouchNode *l_node = reinterpret_cast<SoundTouchNode *>(pNode);
  const double l_ratio = l_node->processor.getInputOutputSampleRatio();
  double l_needed = static_cast<double>(outputFrameCount);
  if (l_ratio > 0.0)
  {
    l_needed = static_cast<double>(outputFrameCount) / l_ratio;
  }
  if (l_needed < 1.0)
  {
    l_needed = 1.0;
  }
  *pInputFrameCount = static_cast<ma_uint32>(l_needed + 0.999999);
  return MA_SUCCESS;
}

ma_node_vtable g_soundtouch_vtable = {
    soundtouch_process,
    soundtouch_get_required_input_frames,
    1,
    1,
    MA_NODE_FLAG_DIFFERENT_PROCESSING_RATES,
};
} // namespace

ma_result audio_backend::soundtouch_node_init(ma_node_graph *p_graph, ma_uint32 p_channels, ma_uint32 p_sample_rate,
                                              ma_node **p_node)
{
  if (p_graph == nullptr || p_node == nullptr || p_channels == 0)
  {
    return MA_INVALID_ARGS;
  }

  SoundTouchNode *l_node = new (std::nothrow) SoundTouchNode();
  if (l_node == nullptr)
  {
    return MA_OUT_OF_MEMORY;
  }

  l_node->channels = p_channels;
  l_node->processor.setChannels(p_channels);
  l_node->processor.setSampleRate(p_sample_rate);
  l_node->processor.setSetting(SETTING_USE_QUICKSEEK, 0);
  l_node->processor.setSetting(SETTING_USE_AA_FILTER, 1);

  ma_node_config l_config = ma_node_config_init();
  l_config.vtable = &g_soundtouch_vtable;
  l_config.pInputChannels = &l_node->channels;
  l_config.pOutputChannels = &l_node->channels;

  const ma_result l_result = ma_node_init(p_graph, &l_config, nullptr, &l_node->base);
  if (l_result != MA_SUCCESS)
  {
    delete l_node;
    return l_result;
  }

  *p_node = reinterpret_cast<ma_node *>(l_node);
  return MA_SUCCESS;
}

void audio_backend::soundtouch_node_uninit(ma_node *p_node)
{
  if (p_node == nullptr)
  {
    return;
  }
  SoundTouchNode *l_node = reinterpret_cast<SoundTouchNode *>(p_node);
  ma_node_uninit(&l_node->base, nullptr);
  delete l_node;
}

void audio_backend::soundtouch_node_set_pitch(ma_node *p_node, float p_semitones)
{
  if (p_node == nullptr)
  {
    return;
  }
  SoundTouchNode *l_node = reinterpret_cast<SoundTouchNode *>(p_node);
  l_node->pitch_semitones.store(p_semitones);
}

void audio_backend::soundtouch_node_set_tempo(ma_node *p_node, float p_tempo)
{
  if (p_node == nullptr)
  {
    return;
  }
  if (p_tempo <= 0.0f)
  {
    p_tempo = 0.01f;
  }
  SoundTouchNode *l_node = reinterpret_cast<SoundTouchNode *>(p_node);
  l_node->tempo.store(p_tempo);
}

bool audio_backend::soundtouch_node_is_drained(ma_node *p_node)
{
  if (p_node == nullptr)
  {
    return true;
  }
  SoundTouchNode *l_node = reinterpret_cast<SoundTouchNode *>(p_node);
  return l_node->flushed.load() && l_node->remaining_frames.load() == 0;
}
