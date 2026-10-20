#ifndef APP_H
#define APP_H

#include "core/audio/audio.h"
#include "core/speech/stt.h"

typedef struct {
  const char    *sound_path;
  SpeechContext *speech;
  Audio         *capture;
  Audio         *playback;
} App;

int init_app(App *app, const char *model_path, const char *sound_path);
void update_app(App *app);
void close_app(App *app);

#endif // APP_H
