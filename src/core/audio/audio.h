#ifndef AUDIO_H
#define AUDIO_H

#include "miniaudio.h"

typedef struct {
  ma_device device;
  ma_decoder decoder;
  int status;
} Audio;

Audio *create_device(ma_device_type type, ma_format format,
                      ma_uint32 channels,
                      ma_uint32 sample_rate,
                      ma_device_data_proc callback,
                      void *user_data);
void start_device(Audio *ctx, const char *path);
void stop_device(Audio *ctx);
void delete_device(Audio *ctx);

#endif // AUDIO_H

