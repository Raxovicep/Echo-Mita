#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include "core/audio/audio.h"
#include "core/speech/stt.h"

typedef struct {
  const char    *sound_path;
  SpeechContext *speech;
  Audio         *capture;
  Audio         *playback;
  bool playing;
} App;

int app_init(App *app, const char *model_path, const char *sound_path);
void app_update(App *app);
void app_close(App *app);

#endif // APP_H
