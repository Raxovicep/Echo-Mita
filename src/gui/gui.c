#include "gui.h"
#include "build.h"
#include <raylib.h>

void gui_init(void) { InitWindow(800, 600, "Mita"); }
void gui_draw(void) {
  BeginDrawing();
  ClearBackground(RED);
#ifdef GIT_HASH
  DrawText(GIT_HASH, 25, 15, 20, GREEN);
#endif
#ifdef COMPILER_VERSION
  DrawText(COMPILER_VERSION, 25, 35, 20, GREEN);
#endif
#ifdef BUILD_TIME
  DrawText(BUILD_TIME, 25, 55, 20, GREEN);
#endif
  EndDrawing();
}
void gui_close(void) { CloseWindow(); }
