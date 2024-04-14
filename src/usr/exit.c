#include "libc/stdio.h"

#include "shell.h"

int usr_exit(int argc, const char* argv[]) {
    shell_exit();

    return EXIT_SUCCESS;
}