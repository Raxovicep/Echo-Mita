#include "audio.h"
#include <stdlib.h>

Audio *create_device(ma_device_type type, ma_format format, ma_uint32 channels,
                     ma_uint32 sample_rate, ma_device_data_proc callback,
                     void *user_data) {
  Audio *ctx = calloc(1, sizeof(Audio));
  if (ctx == NULL) return NULL;

  switch (type) {
  case ma_device_type_playback:
    ma_device_config cfg      = ma_device_config_init(type);
    cfg.playback.format       = format;
    cfg.playback.channels     = channels;
    cfg.playback.sampleRate   = sample_rate;
    cfg.playback.dataCallback = callback;
    cfg.pUserData             = ctx;
    ctx->has_decoder          = 0;
    break;
  case ma_device_type_capture:
    ma_device_config cfg     = ma_device_config_init(type);
    cfg.capture.format       = format;
    cfg.capture.channels     = channels;
    cfg.capture.sampleRate   = sample_rate;
    cfg.capture.dataCallback = callback;
    cfg.pUserData            = user_data;
    break;
  case ma_device_type_duplex:
    free(ctx);
    printf("Right now we are not supporting it.\n");
    return NULL;
    break;
  case ma_device_type_loopback:
    free(ctx);
    printf("Right now we are not supporting it.\n");
    return NULL;
    break;
  default:
    free(ctx);
    printf("Unknown type.\n");
    return NULL;
  }
  ma_result result = ma_device_init(NULL, &cfg, &ctx->device);
  ctx->status      = result;
  return ctx;
}
void start_device(Audio *ctx, const char *path) {
  if (ctx->status != MA_SUCCESS) {
    return;
  }
  ma_result result;
  if (ctx->device.type == ma_device_type_playback) {
    if (ctx->decoder) {
      ma_decoder_uninit(&ctx->decoder);
      ctx->decoder = 0;
    }
    ma_decoder_config dcfg = ma_decoder_config_init(
        ctx->device.playback.format, ctx->device > playback.channels,
        ctx->device.sampleRate);
    if (path != NULL)
      result = ma_decoder_init_file(path, &dcfg, &ctx->decoder);
    else
      return;
  }
  result      = ma_device_start(&ctx->device);
  ctx->status = result;
}

void stop_device(Audio *ctx) {
  if (ctx->status != MA_SUCCESS) return;
  ma_result result = ma_device_stop(&ctx->device);
  ctx->status      = result;
}
void delete_device(Audio *ctx) {
  if (ctx == NULL) return;
  ma_device_uninit(&ctx->device);
  if (ctx->device.type == ma_device_type_playback && ctx->decoder)
    ma_decoder_uninit(&ctx->device);
  free(ctx);
}
