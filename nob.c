#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "ext/nob.h"


#define SRC_FOLDER "src/"
#define BUILD_FOLDER "build/"

int main(int argc, char **argv) {
  GO_REBUILD_URSELF(argc, argv);
  Cmd cmd = {0};
  cmd_append(&cmd, "gcc");
  cmd_append(&cmd, SRC_FOLDER "main.c");
  cmd_append(&cmd, SRC_FOLDER "audio/audio_capture.c");
  cmd_append(&cmd, SRC_FOLDER "audio/audio_playback.c");
  cmd_append(&cmd, SRC_FOLDER "parser-json/parser.c");
  cmd_append(&cmd, SRC_FOLDER "speech/speech_recognition.c");
  cmd_append(&cmd, "-o", BUILD_FOLDER "Mita");
  cmd_append(&cmd, "-Wall", "-Wextra", "-pedantic", "-lm", "-ldl",
                 "-lpthread", "-lvosk", "-ljson-c");
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--debug") == 0) {
      cmd_append(&cmd, "-ggdb");
      cmd_append(&cmd, "-fsanitize=address");
    }
    if (strcmp(argv[i], "--release") == 0) {
      cmd_append(&cmd, "-O2");
      cmd_append(&cmd, "-march=native");
    }
  }
  cmd_append(&cmd, "-lraylib");
  if (!cmd_run(&cmd))
    return 1;
  return 0;
}
