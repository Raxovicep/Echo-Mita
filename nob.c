#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "ext/nob.h"

#define FLAG_IMPLEMENTATION
#include "ext/flag.h"

#define SRC_FOLDER "src/"
#define BUILD_FOLDER "build/"

#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>

void cc(void) {
  cmd_append(&cmd, "gcc");
  cmd_append(&cmd, "-Wall", "-Wextra", "-pedantic");
}

void usage(FILE *stream) {
  fprintf(stream, "Usage: ./nob [OPTIONS] [--] [ARGS]]n");
  fprintf(stream, "OPTIONS:\n");
  flag_print_options(stream);
} 

int main(int argc, char **argv) {
  bool *help = flag_bool("help", false, "Prints the help to stdout and exit with 0");
  bool dev      = false;
  bool run      = false;
  bool no_build = false;

  flag_bool_var(&dev, "dev", false, "Compile to dev mode");
  flag_bool_var(&run, "run", false, "Run the program");
  flag_bool_var(&no_build, "no-build", false, "Do not build a program");

  if (!flag_parse(argc, argv)) {
    usage(stderr);
    flag_print_error(stderr);
    return 1;
  }

  if (*help) {
    usage(stdout);
    return 0;
  }
  
  char output_file_path[] = BUILD_FOLDER"Mita";

  GO_REBUILD_URSELF(argc, argv);
  if (!no_build) {
    cc();
    cmd_append(&cmd, SRC_FOLDER "main.c");
    cmd_append(&cmd, SRC_FOLDER "audio/audio_capture.c");
    cmd_append(&cmd, SRC_FOLDER "audio/audio_playback.c");
    cmd_append(&cmd, SRC_FOLDER "parser-json/parser.c");
    cmd_append(&cmd, SRC_FOLDER "speech/speech_recognition.c");
    cmd_append(&cmd, "-o", BUILD_FOLDER "Mita");
    cmd_append(&cmd, "-ldl", "-lpthread", "-ljson-c", "-lm");
    cmd_append(&cmd, "-lvosk");
    cmd_append(&cmd, "-lraylib");

    if (dev) {
      cmd_append(&cmd, "-ggdb");
      cmd_append(&cmd, "-fsanitize=address");
      cmd_append(&cmd, "-fsanitize=undefined");
      cmd_append(&cmd, "-fno-sanitize-recover=all");
      cmd_append(&cmd, "-fsanitize=float-divide-by-zero");
      cmd_append(&cmd, "-fsanitize=float-cast-overflow"); 
      cmd_append(&cmd, "-fno-sanitize=null");
      cmd_append(&cmd, "-fno-sanitize=alignment");
    } else {
      cmd_append(&cmd, "-O2");
      cmd_append(&cmd, "-march=native");
    }
    if (!cmd_run(&cmd)) return 1;
  }
  if (run) {
    cmd_append(&cmd, temp_sprintf("./%s", output_file_path));
    if (!cmd_run(&cmd)) return 2;
  }
  return 0;
}
