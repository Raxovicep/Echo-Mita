#include <assert.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "ext/nob.h"

#define FLAG_IMPLEMENTATION
#include "ext/flag.h"

#define SRC_FOLDER "src/"
#define BUILD_FOLDER "build/"
#define EXT "ext/"

#include <stdbool.h>
#include <sys/stat.h>

typedef enum { CC, GCC, CLANG, TCC, __compiler_count } Compiler;

typedef struct {
  const char *src;
  const char *obj;
  const char *macro;
} Source;

static_assert(__compiler_count == 4, "Amount of compilers have changed");
const char *compiler_names[__compiler_count] = {
    [CC]    = "cc",
    [GCC]   = "gcc",
    [CLANG] = "clang",
    [TCC]   = "tcc",
};

const Source sources[] = {
    {SRC_FOLDER "core/audio/audio.c", BUILD_FOLDER "audio.o"},
    {SRC_FOLDER "core/parser-json/parser.c", BUILD_FOLDER "parser.o"},
    {SRC_FOLDER "core/speech/stt.c", BUILD_FOLDER "stt.o"},
    {SRC_FOLDER "main.c", BUILD_FOLDER "main.o"},
};

const Source stb_libs[] = {
    {EXT "miniaudio.h", BUILD_FOLDER "miniaudio.o",
     "-DMINIAUDIO_IMPLEMENTATION"},
};
const Source libs[] = {{EXT "raylib/Linux-x86_64/include/raylib.h",
                        EXT "raylib/Linux-x86_64/lib/", "-l:libraylib.a"},
                       {EXT "vosk/Linux-x86_64/include/vosk_api.h",
                        EXT "vosk/Linux-x86_64/lib/", "-l:libvosk.so"}};

const size_t sources_count  = sizeof(sources) / sizeof(sources[0]);
const size_t stb_libs_count = sizeof(stb_libs) / sizeof(stb_libs[0]);
const size_t libs_count     = sizeof(libs) / sizeof(libs[0]);

bool dev = false;

bool set_compiler_by_name(const char *name, Compiler *compiler);
void cc(Cmd *cmd, Compiler compiler);
void add_dev_flags(Cmd *cmd);

bool ensure_directory_exist(const char *path);
char *get_current_date(void);
bool make_build_h(Compiler compiler);
bool is_need_rebuild(const char *src, const char *obj);
bool compile_source(const Source *source, Compiler compiler);
bool compile_stb_libs(const Source *stb, Compiler compiler);
bool link_program(Compiler compiler);
bool build(Compiler compiler, bool is_force);
void usage(FILE *stream);

int main(int argc, char **argv) {
  GO_REBUILD_URSELF(argc, argv);
  bool *help =
      flag_bool("help", false, "Prints the help to stdout and exit with 0");
  bool run            = false;
  bool no_build       = false;
  bool force          = false;
  char *compiler_name = NULL;

  flag_bool_var(&dev, "dev", false, "Compile in dev mode");
  flag_bool_var(&run, "run", false, "Run the program with provided arguments");
  flag_bool_var(&no_build, "no-build", false, "Do not build a program");
  flag_bool_var(&force, "force", false, "Force build a program");
  flag_str_var(&compiler_name, "cc", "gcc", "Compiler to use");

  if (!flag_parse(argc, argv)) {
    usage(stderr);
    flag_print_error(stderr);
    return 1;
  }

  Compiler compiler;
  if (!set_compiler_by_name(compiler_name, &compiler)) {
    nob_log(ERROR, "Unknown compiler \"%s\"", compiler_name);
    nob_log(ERROR, "Supported compilers:");
    for (int i = 0; i < __compiler_count; ++i) {
      nob_log(ERROR, "  %s", compiler_names[i]);
    }
    return 1;
  }

  argc = flag_rest_argc();
  argv = flag_rest_argv();

  if (*help) {
    usage(stdout);
    return 0;
  }
  if (!no_build && !build(compiler, force)) return 1;
  if (run) {
    Cmd cmd = {0};
    cmd_append(&cmd, "./" BUILD_FOLDER "Mita");
    da_append_many(&cmd, argv, argc);
    if (!cmd_run(&cmd)) return 3;
  }
  return 0;
}

bool set_compiler_by_name(const char *name, Compiler *compiler) {
  for (int i = 0; i < __compiler_count; ++i) {
    if (strcmp(name, compiler_names[i]) == 0) {
      *compiler = i;
      return true;
    }
  }
  return false;
}

void cc(Cmd *cmd, Compiler compiler) {
  switch (compiler) {
  case CC:
    cmd_append(cmd, "cc");
    break;
  case GCC:
    cmd_append(cmd, "gcc");
    break;
  case CLANG:
    cmd_append(cmd, "clang");
    break;
  case TCC:
    cmd_append(cmd, "tcc");
    break;
  case __compiler_count:
  default:
    UNREACHABLE("Compiler");
  }

  cmd_append(cmd, "-Wall");
  cmd_append(cmd, "-Wextra");
  cmd_append(cmd, "-pedantic");
  cmd_append(cmd, "-Wswitch-enum");
  cmd_append(cmd, "-Wno-unused-function");
}

void add_dev_flags(Cmd *cmd) {
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

bool ensure_directory(const char *path) {
  if (mkdir_if_not_exists(path)) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
  }

  return false;
}

char *get_current_date(void) {
  static const char *WEEKDAYS[] = {
      "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
  };

  static const char *MONTHS[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };

  time_t rawtime;
  time(&rawtime);
  struct tm *timeinfo = localtime(&rawtime);

  String_Builder sb = {0};
  sb_appendf(&sb, "%s, ", ARRAY_GET(WEEKDAYS, timeinfo->tm_wday));
  sb_appendf(&sb, "%02d ", timeinfo->tm_mday);
  sb_appendf(&sb, "%s ", ARRAY_GET(MONTHS, timeinfo->tm_mon));
  sb_appendf(&sb, "%04d ", timeinfo->tm_year + 1900);
  sb_appendf(&sb, "%02d:", timeinfo->tm_hour);
  sb_appendf(&sb, "%02d:", timeinfo->tm_min);
  sb_appendf(&sb, "%02d ", timeinfo->tm_sec);
  sb_appendf(&sb, "%s", timeinfo->tm_zone);
  sb_append_null(&sb);
  return sb.items;
}

bool make_build_h(Compiler compiler) {
  Cmd cmd           = {0};
  String_Builder sb = {0};
  sb_appendf(&sb, "#ifndef BUILD_H_\n");
  sb_appendf(&sb, "#define BUILD_H_\n");
  sb_appendf(&sb, "\n");
  cmd_append(&cmd, "git");
  cmd_append(&cmd, "rev-parse");
  cmd_append(&cmd, "--short");
  cmd_append(&cmd, "HEAD");
  if (cmd_run(&cmd, .stdout_path = BUILD_FOLDER "git_hash.txt")) {
    sb_appendf(&sb, "#define GIT_HASH \"");
    if (!read_entire_file(BUILD_FOLDER "git_hash.txt", &sb)) return false;
    while (sb.count > 0 && isspace(da_last(&sb))) {
      UNUSED(da_pop(&sb));
    }
    sb_appendf(&sb, "\"\n");
  } else {
    nob_log(WARNING, "git hash will not be included in `Mita version`");
  }

  String_Builder sb_compiler_version = {0};
  switch (compiler) {
  case CC:
    cmd_append(&cmd, "cc", "--version");
    break;
  case GCC:
    cmd_append(&cmd, "gcc", "--version");
    break;
  case CLANG:
    cmd_append(&cmd, "clang", "--version");
    break;
  case TCC:
    cmd_append(&cmd, "tcc", "--version");
    break;
  case __compiler_count:
  default:
    UNREACHABLE("Compiler");
  }
  if (cmd_run(&cmd, .stdout_path = BUILD_FOLDER "compiler-version.txt")) {
    if (!read_entire_file(BUILD_FOLDER "compiler-version.txt",
                          &sb_compiler_version))
      return false;
    String_View compiler_version_txt = sb_to_sv(sb_compiler_version);
    String_View compiler_version =
        sv_chop_by_delim(&compiler_version_txt, '\n');
    sb_appendf(&sb, "#define COMPILER_VERSION \"" SV_Fmt "\"\n",
               SV_Arg(compiler_version));
  } else {
    nob_log(WARNING, "compiler version will not be included in `tatr-version`");
  }
  sb_appendf(&sb, "#define BUILD_TIME \"%s\"\n", get_current_date());

  sb_appendf(&sb, "#endif // BUILD_H_");
  if (!write_entire_file(BUILD_FOLDER "build.h", sb.items, sb.count))
    return false;
  nob_log(INFO, "Generated " BUILD_FOLDER "build.h");
  return true;
}

bool is_need_rebuild(const char *src, const char *obj) {
  struct stat src_stat;
  struct stat obj_stat;

  if (stat(src, &src_stat) != 0) return true;

  if (stat(obj, &obj_stat) != 0) return true;

  return src_stat.st_mtime > obj_stat.st_mtime;
}

bool compile_source(const Source *source, Compiler compiler) {
  Cmd cmd = {0};
  cc(&cmd, compiler);
  add_dev_flags(&cmd);

  cmd_append(&cmd, "-MMD", "-MP");
  cmd_append(&cmd, "-c", source->src);
  cmd_append(&cmd, "-o", source->obj);

  return cmd_run(&cmd);
}

bool compile_stb_libs(const Source *stb, Compiler compiler) {
  Cmd cmd = {0};
  cc(&cmd, compiler);

  cmd_append(&cmd, "-w");
  cmd_append(&cmd, "-c");
  cmd_append(&cmd, "-o", stb->obj);
  cmd_append(&cmd, stb->macro);
  cmd_append(&cmd, "-x", "c");
  cmd_append(&cmd, stb->src);

  return cmd_run(&cmd);
}

bool link_program(Compiler compiler) {
  Cmd cmd = {0};

  nob_log(INFO, "LINK %sMita", BUILD_FOLDER);
  cc(&cmd, compiler);
  add_dev_flags(&cmd);

  for (size_t i = 0; i < sources_count; ++i) {
    cmd_append(&cmd, sources[i].obj);
  }

  for (size_t i = 0; i < stb_libs_count; ++i) {
    cmd_append(&cmd, stb_libs[i].obj);
  }

  cmd_append(&cmd, "-o", BUILD_FOLDER "Mita");
  for (size_t i = 0; i < libs_count; ++i) {
    cmd_append(&cmd, temp_sprintf("-I%s", libs[i].src),
               temp_sprintf("-L%s", libs[i].obj));
    cmd_append(&cmd, libs[i].macro);
  }
  cmd_append(&cmd, "-ldl", "-lpthread", "-ljson-c", "-lm", "-lX11");

  return cmd_run(&cmd);
}

bool build(Compiler compiler, bool is_force) {
  printf("==========STAGE 1==========\n");
  if (!make_build_h(compiler))
    nob_log(WARNING, "Could not create a build.h file");
  printf("===========================\n");
  printf("==========STAGE 2==========\n");
  if (!ensure_directory("build")) return false;
  bool changed = false;

  for (size_t i = 0; i < sources_count; ++i) {
    const Source *source = &sources[i];
    if (!is_need_rebuild(source->src, source->obj) && !is_force) continue;
    if (!compile_source(source, compiler)) return false;
    changed = true;
  }

  bool stb_libs_changed = false;
  for (size_t i = 0; i < stb_libs_count; ++i) {
    const Source *stb = &stb_libs[i];
    if (!is_need_rebuild(stb->src, stb->obj)) continue;
    if (!compile_stb_libs(stb, compiler)) return false;
    stb_libs_changed = true;
  }

  struct stat exe_stat;
  bool is_exec_missing = stat(BUILD_FOLDER "Mita", &exe_stat) != 0;
  if (changed || is_exec_missing || stb_libs_changed) {
    if (!link_program(compiler)) return false;
  } else {
    nob_log(INFO, "Nothing to rebuild.");
  }
  printf("===========================\n");
  return true;
}

void usage(FILE *stream) {
  fprintf(stream, "Usage: ./nob [OPTIONS] [--] [ARGS]\n");
  fprintf(stream, "OPTIONS:\n");
  flag_print_options(stream);
}
