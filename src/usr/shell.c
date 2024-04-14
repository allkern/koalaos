#include "libc/string.h"
#include "libc/stdio.h"
#include "sys/ext2.h"
#include "sys/user.h"
#include "hw/gpu.h"

#include "shell.h"
#include "dir.h"
#include "test.h"
#include "cd.h"
#include "help.h"
#include "cat.h"
#include "color.h"
#include "clear.h"
#include "exit.h"
#include "print.h"
#include "chip8.h"
#include "ver.h"
#include "dump.h"
#include "dis.h"
#include "utime.h"
#include "whoami.h"

static int __exit = 0;

void HACK_malloc_argv() {
    char* base = HACK_MALLOC_BASE;

    for (int i = 0; i < MAX_ARGS; i++) {
        __argv[i] = base;

        base += MAX_ARGLEN;
    }
}

void HACK_argv_fix(int argc) {
    for (int i = argc; i < MAX_ARGS; i++)
        __argv[i] = NULL;
}

void shell_init(void) {
    cmd = curr;

    __exit = 0;

    // "/"
    __envp.cwd[0] = '/';
    __envp.cwd[1] = '\0';

    sef_index = 0;

    SEF_REGISTER(cat);
    SEF_REGISTER(cd);
    SEF_REGISTER(chip8);
    SEF_REGISTER(clear);
    SEF_REGISTER(color);
    SEF_REGISTER(dir);
    SEF_REGISTER(dis);
    SEF_REGISTER(dump);
    SEF_REGISTER(exit);
    SEF_REGISTER(help);
    SEF_REGISTER(print);
    SEF_REGISTER(test);
    SEF_REGISTER(time);
    SEF_REGISTER(ver);
    SEF_REGISTER(whoami);
    SEF_ALIAS(dir, "ls");
    SEF_ALIAS(dir, "l");
    SEF_ALIAS(dis, "d/i");
    SEF_ALIAS(dump, "d/m");

    // Huge hack, we don't use malloc that much anyways
    if (!HACK_MALLOC_BASE) {
        HACK_MALLOC_BASE = malloc(0);

        // Allocate two contiguous blocks
        (void)malloc(0);
    }
}

void shell_register(sef_proto fn, const char* name, const char* desc, int alias) {
    sef[sef_index].alias = alias;
    sef[sef_index].fn = fn;
    sef[sef_index].name = name;
    sef[sef_index++].desc = desc;
}

int getchar_block() {
    char c = getchar();

    while (c == -1)
        c = getchar();

    return c;
}

void init_argv(char* buf) {
    __argc = 0;

    char* ptr = buf;

    while (*ptr != '\0') {
        if (*ptr == '\"') {
            char* arg = __argv[__argc++];

            ptr++;

            while ((*ptr != '\"') && (*ptr != '\0'))
                *arg++ = *ptr++;

            ptr++;

            *arg = '\0';
        } else if (*ptr == ' ') {
            ptr++;
        } else {
            char* arg = __argv[__argc++];

            while ((*ptr != ' ') && (*ptr != '\0'))
                *arg++ = *ptr++;

            *arg = '\0';
        }
    }
}

void shell_exit(void) {
    __exit = 1;
}

int shell_exec(const char* buf) {
    __argc = 0;

    for (int i = 0; i < MAX_SEF_COUNT; i++) {
        if (!sef[i].name)
            continue;

        size_t len = strlen(sef[i].name);
        size_t clen = strlen(buf);

        // if (len != clen)
        //     continue;
 
        if (!strncmp(sef[i].name, buf, len)) {
            HACK_malloc_argv();
            init_argv(buf);
            HACK_argv_fix(__argc);

            // To-do: Do something with return value
            return sef[i].fn(__argc, (const char**)__argv);
        }
    }

    printf("Unknown command \'%s\'\n", buf);
}

void shell_start() {
    char* ptr = cmd;

    while (!__exit) {
        printf("\r%s@%s:%s%c %s",
            user_get_name(),
            "koala",
            __envp.cwd,
            user_is_root() ? '#' : '$',
            cmd
        );

        char c = getchar_block();

        switch (c) {
            // Up arrow
            case 0x11: {
                int len = strlen(cmd);

                gpu_set_xpos(gpu_get_xpos() - len * 8);

                while (len--)
                    gpu_putchar(' ');

                cmd = prev;
                ptr = cmd + strlen(cmd);
            } break;

            // Down arrow
            case 0x12: {
                int len = strlen(cmd);

                gpu_set_xpos(gpu_get_xpos() - len * 8);

                while (len--)
                    gpu_putchar(' ');

                cmd = curr;
                ptr = cmd + strlen(cmd);
            } break;

            // Escape key (clear command)
            case 0x1b: {
                int len = strlen(cmd);

                gpu_set_xpos(gpu_get_xpos() - len * 8);

                while (len--)
                    gpu_putchar(' ');

                ptr = cmd;

                cmd[0] = '\0';
            } break;

            // Enter key pressed
            case '\r': {
                if (cmd[0] == '\0') {
                    putchar('\n');

                    goto done;
                }

                if (cmd != prev)
                    memcpy(prev, cmd, MAX_CMD);

                putchar('\n');

                *ptr = '\0';

                shell_exec(cmd);

                cmd = curr;

                done:

                // Reset command
                cmd[0] = '\0';

                ptr = cmd;
            } break;

            case '\b': {
                if (ptr - cmd) {
                    gpu_set_xpos(gpu_get_xpos() - 8);
                    gpu_putchar(' ');

                    *(--ptr) = '\0';
                }
            } break;

            default: {
                *ptr++ = c;
                *ptr = '\0';
            } break;
        }
    }

    printf("exit\n");
}

char* shell_get_cwd(void) {
    return __envp.cwd;
}

void shell_set_cwd(const char* path) {
    char buf[512];

    shell_get_absolute_path(path, buf, 512);

    memcpy(__envp.cwd, buf, 512);
}

struct sef_desc* shell_get_sef_desc(int i) {
    return &sef[i];
}

void shell_get_absolute_path(char* path, char* buf, size_t size) {
    // printf("path=%s, cwd=%s\n", path, __envp.cwd);

    if (!path) {
        strncpy(buf, __envp.cwd, size);

        return;
    }

    char home[128];

    // Path is already absolute
    if (*path == '/') {
        strncpy(buf, path, size);

        return;
    }

    char* rel = __envp.cwd;

    if (*path == '~') {
        user_get_home_path(home);

        ++path;
        rel = home;
    }

    // printf("rel=%s path=%s (%s%s/)\n", rel, path, rel, path);

    sprintf(buf, "%s%s", rel, path);
}