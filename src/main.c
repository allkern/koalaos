#include "libc/stdio.h"
#include "libc/string.h"

#include "usr/shell.h"
#include "sys/user.h"
#include "hw/gpu.h"
#include "util/bmp.h"

char* input(char* buf, size_t size, int hidden) {
    char* ptr = buf;
    int i = 0;

    while (1) {
        char c = getchar();

        while (c == -1)
            c = getchar();

        switch (c) {
            case '\r': {
                putchar('\n');

                return buf;
            } break;

            case '\b': {
                if (ptr - buf) {
                    if (!hidden) {
                        gpu_set_xpos(gpu_get_xpos() - 8);
                        gpu_putchar(' ');
                        gpu_set_xpos(gpu_get_xpos() - 8);
                    }

                    *(--ptr) = '\0';
                    --i;
                }
            } break;

            // Escape key (clear command)
            case 0x1b: {
                if (hidden) {
                    ptr = buf;

                    buf[0] = '\0';

                    break;
                }

                int len = strlen(buf);

                gpu_set_xpos(gpu_get_xpos() - len * 8);

                while (len--)
                    gpu_putchar(' ');

                gpu_set_xpos(gpu_get_xpos() - len * 8);

                ptr = buf;

                buf[0] = '\0';
            } break;

            default: {
                if (i == size)
                    break;

                ++i;

                if (!hidden)
                    putchar(c);

                *ptr++ = c;
                *ptr = '\0';
            } break;
        }
    }

    return buf;
}

int main() {
    gpu_clear();

    struct bmp_s bmp;

    int i = bmp_open(&bmp, "/usr/user/16bpp.bmp");

    dmem_init();

    printf(
        "open      %u\n"
        "compress  %u\n"
        "signature %c%c\n"
        "filesize  %d\n"
        "dataoff   %d\n"
        "width     %u\n"
        "height    %u\n"
        "bpp       %u\n",
        i,
        bmp.info.i_compression,
        bmp.hdr.h_signature[0],
        bmp.hdr.h_signature[1],
        bmp.hdr.h_file_size,
        bmp.hdr.h_data_offset,
        bmp.info.i_width,
        bmp.info.i_height,
        bmp.info.i_bpp
    );

    puts("Welcome to KoalaOS!\n\nCopyright (C) 2024 Allkern/Lisandro Alarcon\n");

    login:

    char name[USER_NAME_MAXLEN];
    char pass[USER_PASS_MAXLEN];

    name_again:

    int att_count = 3;

    printf("koalaos login: ");

    input(name, USER_NAME_MAXLEN, 0);

    if (!user_lookup_by_name(name)) {
        printf("User \'%s\' does not exist, please try again\n", name);

        goto name_again;
    }

    pass_again:

    printf("password: ");

    input(pass, USER_PASS_MAXLEN, 1);

    if (user_login(name, pass) && att_count) {
        --att_count;

        if (!att_count) {
            printf("Exceeded maximum number of attemps, please try again\n");

            goto name_again;
        }

        puts("Sorry, try again");

        goto pass_again;
    }

    putchar('\n');

    shell_init();
    shell_start();

    goto login;

    return 0;
}