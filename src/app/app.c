#include "app.h"
#include "callback.h"
#include "core/parser-json/parser.h"
#include "nob.h"
#include <string.h>

#define SAMPLE_RATE 16'000

/* Because of App is allocated at the stack in main() we can just set memory to
 * 0*/
int app_init(App *app, const char *model_path, const char *sound_path) {
  memset(app, 0, sizeof *app);
  app->sound_path = sound_path;

  app->speech = create_speech(model_path, SAMPLE_RATE);
  if (!app->speech) {
    nob_log(NOB_ERROR, "could not load model or recognizer not initialized.");
    return -1;
  }

  app->capture = create_device(ma_device_type_capture, ma_format_s16, 1,
                               SAMPLE_RATE, recog_call, app->speech);
  if (app->capture->status != MA_SUCCESS) {
    nob_log(NOB_ERROR, "could not open capture device. Code: %i.",
            app->capture->status);
    goto fail;
  }
  app->playback =
      create_device(ma_device_type_playback, 0, 0, 0, playback, NULL);
  if (app->playback->status != MA_SUCCESS) {
    nob_log(NOB_ERROR, "could not open playback device. Code: %i.",
            app->playback->status);
    goto fail;
  }
  return 0;
fail:
  app_close(app);
  return -1;
}
void app_update(App *app) {
  SpeechContext *s = app->speech;
  if (s->has_result) return;
  s->has_result = 0;

  char *result = parse_json(s->text);
  if (result && strcmp(result, "привет") == 0) {
    stop_device(capture);
    start_device(playback, app->sound_path);
    start_device(capture, NULL);
  }
  free(result);
}

void app_close(App *app) {
  if (app->capture) delete_device(app->capture);
  if (app->playback) delete_device(app->playback);
  if (app->speech) delete_device(app->speech);
  memset(app, 0, sizeof *app);
}
