#ifndef SHELL_H
#define SHELL_H

#include "libc/stddef.h"

#define MAX_PATH 512

// Shell environment
static struct sh_envp {
    char cwd[MAX_PATH];
} __envp;

// Shell embedded functions (help, dir, etc.)
typedef int (*sef_proto)(int, const char* argv[]);

#define MAX_SEF_COUNT 32

static unsigned int sef_index = 0;

static struct sef_desc {
    int alias;
    const char* name;
    const char* desc;
    sef_proto fn;
} sef[MAX_SEF_COUNT];

// CLI
#define MAX_ARGS 16
#define MAX_ARGLEN 128

static int __argc;
static char* __argv[MAX_ARGS];

#define MAX_CMD 256

static char prev[MAX_CMD];
static char curr[MAX_CMD];
static char* cmd;

void shell_init(void);
void shell_register(sef_proto fn, const char* name, const char* desc, int alias);
void shell_start(void);
int shell_exec(const char* args);
char* shell_get_cwd(void);
struct sef_desc* shell_get_sef_desc(int i);
char* shell_get_path(char* path);
void shell_exit(void);

char* shell_get_cwd(void);
void shell_set_cwd(const char* path);
void shell_get_absolute_path(char* path, char* buf, size_t size);

static char* HACK_MALLOC_BASE;

#endif