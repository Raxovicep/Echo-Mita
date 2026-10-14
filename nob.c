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

typedef enum {
  CC,
  GCC,
  CLANG,
  TCC,
  __compiler_count
} Compiler;

typedef struct{
  const char *src;
  const char *obj;
} Source;

static const Source sources[] =  {
  {SRC_FOLDER"main.c", BUILD_FOLDER"main.o"},
  {SRC_FOLDER"audio/audio_capture.c", BUILD_FOLDER"audio_capture.o"},
  {SRC_FOLDER"audio/audio_playback.c", BUILD_FOLDER"audio_playback.o"},
  {SRC_FOLDER"parser-json/parser.c", BUILD_FOLDER"parser.o"},
  {SRC_FOLDER"speech/speech_recognition.c", BUILD_FOLDER"speech_recognition.o"},
};

static const size_t sources_count = sizeof(sources) / sizeof(sources[0]);

static bool dev = false;

void cc(Cmd *cmd) {
  cmd_append(cmd, "gcc");
  cmd_append(cmd, "-Wall", "-Wextra", "-pedantic");
}

static void add_dev_flags(Cmd *cmd)
{
  if (dev) {
    cmd_append(cmd, "-ggdb");
    cmd_append(cmd, "-fsanitize=address");
    cmd_append(cmd, "-fsanitize=undefined");
    cmd_append(cmd, "-fno-sanitize-recover=all");
    cmd_append(cmd, "-fsanitize=float-divide-by-zero");
    cmd_append(cmd, "-fsanitize=float-cast-overflow");
    cmd_append(cmd, "-fno-sanitize=null");
    cmd_append(cmd, "-fno-sanitize=alignment");
  } else {
    cmd_append(cmd, "-O2");
    cmd_append(cmd, "-march=native");
  }
}

static bool ensure_directory(const char *path)
{
  if (mkdir_if_not_exists(path)) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
  }

  return false;
}

static bool is_need_rebuild(const char *src, const char *obj) {
  struct stat src_stat;
  struct stat obj_stat;

  if (stat(src, &src_stat) != 0)
    return true;

  if (stat(obj, &obj_stat) != 0)
    return true;

  return src_stat.st_mtime > obj_stat.st_mtime;
}

static bool compile_source(const Source *source) {
  Cmd cmd = {0};
  cc(&cmd);
  add_dev_flags(&cmd);

  cmd_append(&cmd, "-MMD", "-MP");
  cmd_append(&cmd, "-c", source->src);
  cmd_append(&cmd, "-o", source->obj);

  return cmd_run(&cmd);
}

static bool link_program(void) {
  Cmd cmd = {0};

  cc(&cmd);
  add_dev_flags(&cmd);

  for (size_t i = 0; i < sources_count; ++i) {
    cmd_append(&cmd, sources[i].obj);
  }

  cmd_append(&cmd, "-o", BUILD_FOLDER "Mita");
  cmd_append(&cmd, "-ldl", "-lpthread", "-ljson-c", "-lm");
  cmd_append(&cmd, "-lvosk", "-lraylib");

  nob_log(INFO, "LINK %sMita\n", BUILD_FOLDER);
  return cmd_run(&cmd);
}

static bool build(void) {
  if (!ensure_directory("build")) return false;
  bool changed = false;

  for (size_t i = 0; i < sources_count; ++i) {
    const Source *source = &sources[i];
    if (!is_need_rebuild(source->src, source->obj)) continue;
    if (!compile_source(source)) return false;
    changed = true;
  }
  struct stat exe_stat;
  bool is_exec_missing = stat(BUILD_FOLDER"Mita", &exe_stat) != 0;
  if (changed || is_exec_missing) {
    if (!link_program()) return false;
  } else {
    nob_log(INFO, "Nothing to rebuild.");
  }
  return true;
}

static void usage(FILE *stream) {
  fprintf(stream, "Usage: ./nob [OPTIONS] [--] [ARGS]\n");
  fprintf(stream, "OPTIONS:\n");
  flag_print_options(stream);
} 

int main(int argc, char **argv) {
  bool *help = flag_bool("help", false, "Prints the help to stdout and exit with 0");
  bool run      = false;
  bool no_build = false;

  flag_bool_var(&dev, "dev", false, "Compile in dev mode");
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
  
  GO_REBUILD_URSELF(argc, argv);
  if (!no_build && !build()) return 1;
  if (run) {
    Cmd cmd = {0};
    cmd_append(&cmd, "./"BUILD_FOLDER"Mita");
    if (!cmd_run(&cmd)) return 2;
  }
  return 0;
}
