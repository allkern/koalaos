#include "libc/string.h"
#include "libc/stdio.h"

#include "hw/gpu.h"

int usr_clear(int argc, const char* argv[]) {
    gpu_clear();

    return EXIT_SUCCESS;
}