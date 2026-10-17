#ifndef AUDIO_PLAYBACK
#define AUDIO_PLAYBACK

#include <miniaudio/extras/decoders/libvorbis/miniaudio_libvorbis.h>
#include <miniaudio/miniaudio.h>

typedef struct {
  ma_device device;
  ma_decoder decoder;
  int status;
  int has_decoder;
} AudioPlayback;

AudioPlayback *create_playback(ma_format format, ma_uint32 channles,
                               ma_uint32 sample_rate,
                               ma_device_data_proc data_callback);
void start_playback(AudioPlayback *ctx, const char *path);
void stop_playback(AudioPlayback *ctx);
void delete_playback(AudioPlayback *ctx);
#endif // AUDIO_PLAYBACK
