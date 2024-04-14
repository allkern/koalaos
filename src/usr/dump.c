#include "libc/string.h"
#include "libc/stdio.h"
#include "libc/ctype.h"

#include "hw/gpu.h"

int usr_dump(int argc, const char* argv[]) {
    if (!argv[1])
        return 1;

    char* end = NULL;

    uint32_t addr = strtoul(argv[1], &end, 16);
    int len = 64;
    int colorize = 1;

    for (int i = 2; i < argc; i++) {
        if (!strncmp(argv[i], "-c", 2)) {
            colorize = 0;

            break;
        }
    }

    if (argv[2])
        len = strtoul(argv[2], &end, 0);

    if (!len)
        len = 64;

    // Round to a multiple of 16
    len &= 0xfffffff0;

    uint8_t* ptr = (uint8_t*)addr;

    while (len > 0) {
        for (int i = 0; i < 16; i++) {
            if (colorize && !ptr[i])
                gpu_set_attribute(0x08);

            printf("%02X ", ptr[i]);

            if (colorize && !ptr[i])
                gpu_restore_attribute();
        }

        putchar('|');

        for (int i = 0; i < 16; i++) 
            putchar(isprint(ptr[i]) ? ptr[i] : '.');

        putchar('|');
        putchar('\n');

        ptr += 16;
        len -= 16;
    }

    return EXIT_SUCCESS;
}