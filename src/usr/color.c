#include "libc/string.h"
#include "libc/stdio.h"

#include "hw/gpu.h"

int hexdec(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    
    return 0;
}

int usr_color(int argc, const char* argv[]) {
    if (!argv[1]) {
        gpu_restore_attribute();

        return EXIT_SUCCESS;
    }

    uint8_t attr = (hexdec(argv[1][0]) << 4) | hexdec(argv[1][1]);

    gpu_set_attribute(attr);

    return EXIT_SUCCESS;
}