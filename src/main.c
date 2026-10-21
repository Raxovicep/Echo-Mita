#include "app/app.h"
#include "gui/gui.h"
#include <raylib.h>

#define NOB_IMPLEMENTATION
#include "../ext/nob.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    nob_log(ERROR, "You didn't provide a file to play");
    return 2;
  }

  App app;
  int rc = app_init(&app, "resources/model", argv[1]);
  if (rc != 0) return rc;

  gui_init();
  while (!WindowShouldClose()) {
    app_update(&app);
    gui_draw();
  }
  gui_close();
  app_close(&app);
  return 0;
}
