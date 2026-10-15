#include "audio/audio_capture.h"
#include "audio/audio_playback.h"
#include "parser-json/parser.h"
#include "speech/speech_recognition.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOB_IMPLEMENTATION
#include "../ext/nob.h"

#define SAMPLE_RATE 16000

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
  SpeechContext *rec = (SpeechContext *)pDevice->pUserData;
  // 16-bit mono → 2 bytes per frame
  if (frameCount > INT_MAX / sizeof(ma_int16)) return;
  int bytes = (int)(frameCount * sizeof(ma_int16));

  if (speech_process_audio(rec, pInput, bytes)) {
    strncpy(rec->text, speech_get_result(rec), MAX_TEXT - 1);
    rec->text[MAX_TEXT - 1] = '\0';
    rec->has_result         = 1;
  }
  // else {
  // printf("%s\n", speech_get_partial(rec));
  //}

  (void)pOutput; // unused
}

void playback_callback(ma_device *device, void *output, const void *input,
                       ma_uint32 frameCount) {
  AudioPlayback *ctx = (AudioPlayback *)device->pUserData;

  if (ctx->has_decoder) {
    ma_decoder_read_pcm_frames(&ctx->decoder, output, frameCount, NULL);
  } else {
    // Make silence
    memset(output, 0,
           frameCount * ma_get_bytes_per_frame(device->playback.format,
                                               device->playback.channels));
  }
  (void)input;
}

int main(int argc, char *argv[]) {
  char *model_path = "resources/model";
  int retval       = 0;
  if (argc < 2) {
    nob_log(ERROR, "You don't provide file to play sound");
    return retval = -2;
  }
  SpeechContext *speech_ctx = create_speech(model_path, SAMPLE_RATE);

  if (speech_ctx == NULL) {
    nob_log(ERROR, "could not load model or recognizer not initialized.\n");
    return -1;
  }

  AudioCapture *capture =
      create_capture(ma_format_s16, 1, SAMPLE_RATE, data_callback, speech_ctx);

  if (capture->status != MA_SUCCESS) {
    nob_log(ERROR, "could not open capture device. Code: %i\n",
            capture->status);
    delete_capture(capture);
    delete_speech(speech_ctx);
    return capture->status;
  }
  // Create an audio playback with native setting
  AudioPlayback *playback = create_playback(0, 0, 0, playback_callback);

  if (playback->status != MA_SUCCESS) {
    retval = playback->status;
    nob_log(ERROR, "open playback device. Code: %i\n", retval);
    goto unload;
  }

  printf("Device name: %s\n", capture->device.capture.name);
  start_capture(capture);
  if (capture->status != MA_SUCCESS) {
    retval = capture->status;
    nob_log(ERROR, "could not start capture device. Code: %i\n", retval);
    goto unload;
  }

  InitWindow(800, 600, "Mita");
  while (!WindowShouldClose()) {
    if (speech_ctx->has_result) {
      speech_ctx->has_result = 0;
      char *result           = parse_json(speech_ctx->text);
      if (result != NULL && strcmp(result, "привет") == 0) {
        stop_capture(capture);
        start_playback(playback, argv[1]);
        start_capture(capture);
      }
      free(result);
    }
    BeginDrawing();
    ClearBackground(RED);
    EndDrawing();
  }
  retval = 0;
unload:
  delete_capture(capture);
  delete_playback(playback);
  delete_speech(speech_ctx);
  CloseWindow();
  return retval;
}
