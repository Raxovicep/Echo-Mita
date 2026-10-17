#include "audio_playback.h"
#include <miniaudio/extras/decoders/libvorbis/miniaudio_libvorbis.h>
#include <miniaudio/miniaudio.h>
#include <stdio.h>
#include <stdlib.h>

AudioPlayback *create_playback(ma_format format, ma_uint32 channels,
                               ma_uint32 sample_rate,
                               ma_device_data_proc data_callback) {
  AudioPlayback *ctx = malloc(sizeof(AudioPlayback));

  if (ctx == NULL)
    return NULL;

  ma_device_config device_config =
      ma_device_config_init(ma_device_type_playback);

  device_config.playback.format = format;
  device_config.playback.channels = channels;
  device_config.sampleRate = sample_rate;
  device_config.dataCallback = data_callback;
  device_config.pUserData = ctx;

  ma_result result = ma_device_init(NULL, &device_config, &ctx->device);
  ctx->status = result;
  ctx->has_decoder = 0;
  return ctx;
}

void start_playback(AudioPlayback *ctx, const char *path) {
  if (ctx->status != MA_SUCCESS)
    return;
  ma_device_start(&ctx->device);
  if (ctx->has_decoder) {
    ma_decoder_uninit(&ctx->decoder);
    ctx->has_decoder = 0;
  }
  ma_decoder_config decoder_config = ma_decoder_config_init(
      ctx->device.playback.format, ctx->device.playback.channels,
      ctx->device.sampleRate);
  ma_result result = ma_decoder_init_file(path, &decoder_config, &ctx->decoder);

  if (result == MA_SUCCESS) {
    ctx->has_decoder = 1;
  }
  ctx->status = result;
}

void stop_playback(AudioPlayback *ctx) {
  if (ctx->status != MA_SUCCESS)
    return;
  ma_result result = ma_device_stop(&ctx->device);
  ctx->status = result;
}

void delete_playback(AudioPlayback *ctx) {
  if (ctx == NULL)
    return;
  ma_device_uninit(&ctx->device);

  if (ctx->has_decoder != 0)
    ma_decoder_uninit(&ctx->decoder);

  free(ctx);
}
