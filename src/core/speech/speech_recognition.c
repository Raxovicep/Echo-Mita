#include "speech_recognition.h"
#include <stdlib.h>
#include <vosk_api.h>

SpeechContext *create_speech(const char *model_path, int sample_rate) {
  // Allocate memory for context
  SpeechContext *ctx = malloc(sizeof(SpeechContext));
  if (ctx == NULL)
    return NULL;

  // Load model
  ctx->model = vosk_model_new(model_path);
  if (ctx->model == NULL) {
    free(ctx);
    return NULL;
  }

  // Load recognizer
  ctx->recognizer = vosk_recognizer_new(ctx->model, sample_rate);
  if (ctx->recognizer == NULL) {
    vosk_model_free(ctx->model);
    free(ctx);
    return NULL;
  }

  ctx->text[0] = '\0';
  ctx->has_result = 0;

  return ctx;
}

int speech_process_audio(SpeechContext *ctx, const void *data, int size) {
  if (ctx == NULL || data == NULL) {
    return -2;
  }
  return vosk_recognizer_accept_waveform(ctx->recognizer, data, size);
}

const char *speech_get_result(SpeechContext *ctx) {
  return vosk_recognizer_result(ctx->recognizer);
}

const char *speech_get_partial(SpeechContext *ctx) {
  return vosk_recognizer_partial_result(ctx->recognizer);
}

void delete_speech(SpeechContext *ctx) {
  if (ctx == NULL) {
    return;
  }
  if (ctx->recognizer) {
    vosk_recognizer_free(ctx->recognizer);
  }
  if (ctx->model) {

    vosk_model_free(ctx->model);
  }
  free(ctx);
}
