#include <string.h>
#define MINIAUDIO_IMPLEMENTATION
#include "audio/audio_capture.h"
#include "audio/audio_playback.h"
#include "parser-json/parser.h"
#include "speech/speech_recognition.h"
#include <raylib.h>
#include <stdio.h>

#define SAMPLE_RATE 16000

#define ERROR_Tmp "ERROR: could not"

// Callback function
void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
  SpeechContext *rec = (SpeechContext *)pDevice->pUserData;
  // 16-bit mono → 2 bytes per frame
  int bytes = frameCount * 2;

  if (speech_process_audio(rec, pInput, bytes)) {
    // printf("%s\n", speech_get_result(rec));
    strncpy(rec->text, speech_get_result(rec), MAX_TEXT - 1);
    rec->text[MAX_TEXT - 1] = '\0';
    rec->has_result = 1;
  }
  // else {
  // printf("%s\n", speech_get_partial(rec));
  //}

  (void)pOutput; // unused
}

void playback_callback(ma_device *device, void *output, const void *input,
                       ma_uint32 frameCount) {
  AudioPlayback *ctx = (AudioPlayback *)device->pUserData;

  // Read pcm frames
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
  InitWindow(800, 600, "Mita");
  // Path
  char *model_path = "resources/model";
  int retval = 0;

  SpeechContext *speech_ctx = create_speech(model_path, SAMPLE_RATE);

  // Check context is loaded
  if (speech_ctx == NULL) {
    fprintf(stderr, ERROR_Tmp " load model or recognizer not initialized.\n");
    return -1;
  }

  AudioCapture *capture =
      create_capture(ma_format_s16, 1, SAMPLE_RATE, data_callback, speech_ctx);

  // Check device is created
  if (capture->status != MA_SUCCESS) {
    fprintf(stderr, ERROR_Tmp " open capture device. Code: %i\n",
            capture->status);
    delete_capture(capture);
    delete_speech(speech_ctx);
    return capture->status;
  }
  // Create an audio playback with native setting
  AudioPlayback *playback = create_playback(0, 0, 0, playback_callback);

  if (playback->status != MA_SUCCESS) {
    retval = playback->status;
    fprintf(stderr, ERROR_Tmp " open playback device. Code: %i\n", retval);
    goto unload;
  }

  printf("Press Space to stop...");
  printf("Device name: %s\n", capture->device.capture.name);
  // Check device is started
  start_capture(capture);
  if (capture->status != MA_SUCCESS) {
    retval = capture->status;
    fprintf(stderr, ERROR_Tmp " start capture device. Code: %i\n", retval);
    goto unload;
  }

  while (!WindowShouldClose()) {
    if (speech_ctx->has_result) {
      speech_ctx->has_result = 0;
      char *result = parse_json(speech_ctx->text);
      if (strcmp(result, "привет") == 0) {

        // RLAPI float GetMusicTimeLength(Music music); // Get music time length
        // (in seconds)
        stop_capture(capture);
        start_playback(playback, argv[1]);
        start_capture(capture);
        free(result);
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
