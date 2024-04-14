#include "libc/stdio.h"

#include "shell.h"

int usr_help(int argc, const char* argv[]) {
    int index = 0;

    struct sef_desc* desc = shell_get_sef_desc(index++);

    while (desc->name) {
        if (!desc->alias)
            printf(" %-8s - %s\n", desc->name, desc->desc);

        desc = shell_get_sef_desc(index++);
    }

    return EXIT_SUCCESS;
}