#include "audio_capture.h"
#include <miniaudio/miniaudio.h>
#include <stdlib.h>

AudioCapture *create_capture(ma_format format, ma_uint32 channles,
                             ma_uint32 sample_rate,
                             ma_device_data_proc data_callback,
                             void *user_data) {

  AudioCapture *ctx = malloc(sizeof(AudioCapture));
  if (ctx == NULL) {
    return NULL;
  }
  ma_device_config device_config =
      ma_device_config_init(ma_device_type_capture);

  device_config.capture.format   = format;
  device_config.capture.channels = channles;
  device_config.sampleRate       = sample_rate;
  device_config.dataCallback     = data_callback;
  device_config.pUserData        = user_data;

  ma_result result = ma_device_init(NULL, &device_config, &ctx->device);
  ctx->status      = result;
  return ctx;
}

// Methods
void start_capture(AudioCapture *ctx) {
  ma_result result = ma_device_start(&ctx->device);
  ctx->status      = result;
}

void stop_capture(AudioCapture *ctx) {
  ma_result result = ma_device_stop(&ctx->device);
  ctx->status      = result;
}

void delete_capture(AudioCapture *ctx) {
  ma_device_uninit(&ctx->device);
  free(ctx);
}
