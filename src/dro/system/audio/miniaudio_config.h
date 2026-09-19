#pragma once

#define MA_NO_ENCODING

#include <miniaudio/miniaudio.h>

namespace audio_backend
{
ma_decoding_backend_vtable **custom_backends();
ma_uint32 custom_backend_count();

ma_result decoder_init_with_tell(ma_decoder_read_proc p_on_read, ma_decoder_seek_proc p_on_seek,
                                ma_decoder_tell_proc p_on_tell, void *p_user_data,
                                const ma_decoder_config *p_config, ma_decoder *p_decoder);

ma_result reverb_node_init(ma_node_graph *p_graph, ma_uint32 p_channels, ma_uint32 p_sample_rate, ma_node **p_node);
void reverb_node_uninit(ma_node *p_node);

ma_result soundtouch_node_init(ma_node_graph *p_graph, ma_uint32 p_channels, ma_uint32 p_sample_rate, ma_node **p_node);
void soundtouch_node_uninit(ma_node *p_node);
void soundtouch_node_set_pitch(ma_node *p_node, float p_semitones);
void soundtouch_node_set_tempo(ma_node *p_node, float p_tempo);
bool soundtouch_node_is_drained(ma_node *p_node);
} // namespace audio_backend
