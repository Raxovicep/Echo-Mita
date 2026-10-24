#include "gui.h"
#include "build.h"
#include <raylib.h>

static Font fonts        = {0};
static const char *msg   = "Hello";
static const int spacing = 2;
static Vector2 positions = {0};
Color colors             = GOLD;

void gui_init(void) {
  InitWindow(800, 600, "Mita");

  fonts = LoadFont("resources/Font/roundedmplus1c/RoundedMplus1c-Black.ttf");
  fonts.baseSize     = 64;
  Vector2 text_width = MeasureTextEx(fonts, msg, fonts.baseSize, spacing);
  positions.x        = (GetScreenWidth() * .5f) - (text_width.x * .5f);
  positions.y        = (GetScreenHeight() * .5f) - (text_width.y * .5f);

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
}
void gui_draw(void) {
  BeginDrawing();
  ClearBackground(RED);
#ifdef GIT_HASH
  DrawTextEx(fonts, GIT_HASH, (Vector2){25, 15}, 32, spacing, GREEN);
#endif
#ifdef COMPILER_VERSION
  DrawTextEx(fonts, COMPILER_VERSION, (Vector2){25, 35}, 32, spacing, GREEN);
#endif
#ifdef BUILD_TIME
  DrawTextEx(fonts, BUILD_TIME, (Vector2){25, 55}, 32, spacing, GREEN);
#endif
  DrawTextEx(fonts, msg, positions, 128, (float)spacing, colors);
  EndDrawing();
}
void gui_close(void) {
  UnloadFont(fonts);
  // LoadModel
  CloseWindow();
}
