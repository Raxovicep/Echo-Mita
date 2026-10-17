#ifndef SPEECH_RECOGNITION
#define SPEECH_RECOGNITION

#include <vosk_api.h>

#define MAX_TEXT 512

typedef struct {
  VoskModel *model;
  VoskRecognizer *recognizer;
  char text[MAX_TEXT];
  int has_result;
} SpeechContext;

SpeechContext *create_speech(const char *model_path, int sample_rate);

int speech_process_audio(SpeechContext *ctx, const void *data, int size);

const char *speech_get_result(SpeechContext *ctx);

const char *speech_get_partial(SpeechContext *ctx);
void delete_speech(SpeechContext *ctx);

#endif // SPEECH_RECOGNITION
