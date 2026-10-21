#include "callback.h"
#include "core/speech/stt.h"
#include <string.h>

void recog_call(ma_device *pDevice, void *pOutput, const void *pInput,
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

void playback(ma_device *device, void *output, const void *input,
              ma_uint32 frameCount) {
  Audio *ctx = (Audio *)device->pUserData;

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
