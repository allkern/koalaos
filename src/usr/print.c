#include "libc/stdio.h"

int usr_print(int argc, const char* argv[]) {
    for (int i = 1; i < argc; i++)
        puts(argv[i]);

    return EXIT_SUCCESS;
}