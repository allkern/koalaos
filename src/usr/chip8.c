#include "libc/string.h"
#include "libc/stdio.h"
#include "sys/ext2.h"
#include "hw/uart.h"
#include "hw/gpu.h"
#include "c8.h"

#include "util/mmio.h"
#include "shell.h"

int get_c8_key(int k) {
    if (k == '1') return 0x1;
    if (k == '2') return 0x2;
    if (k == '3') return 0x3;
    if (k == '4') return 0xc;
    if (k == 'q') return 0x4;
    if (k == 'w') return 0x5;
    if (k == 'e') return 0x6;
    if (k == 'r') return 0xd;
    if (k == 'a') return 0x7;
    if (k == 's') return 0x8;
    if (k == 'd') return 0x9;
    if (k == 'f') return 0xe;
    if (k == 'z') return 0xa;
    if (k == 'x') return 0x0;
    if (k == 'c') return 0xb;
    if (k == 'v') return 0xf;
    return -1;
}

chip8_t c8;
uint8_t screen[64 * 32];
uint8_t mem[0x1000];

int usr_chip8(int argc, const char* argv[]) {
    char buf[EXT2_SECTOR_SIZE * 2];

    struct ext2_inode inode;

    if (!argv[1]) {
        printf("No input supplied\n");

        return EXIT_FAILURE;
    }

    char path[MAX_PATH];

    shell_get_absolute_path(argv[1], path, MAX_PATH);

    struct ext2_fd file;

    if (ext2_fopen(&file, path, "rb")) {
        printf("Couldn't find path \'%s\'\n", path);

        return EXIT_FAILURE;
    }

    ext2_fread(&file, buf, file.inode.s_sizel);
    ext2_fclose(&file);

    gpu_clear();

    c8_init(&c8, screen, mem);
    c8_load_program(&c8, buf, file.inode.s_sizel);

    int last_k = -1;

    while (1) {
        // Wait for GPU IRQ
        while (!(mmio_read_32(0x9f801070) & 1));

        // Acknowledge GPU IRQ
        mmio_write_32(0x9f801070, ~1ul);

        if (mmio_read_8(UART_LSR) & LSR_RX_READY) {
            int data = mmio_read_8(UART_RHR);

            // Exit emulator
            if (data == 0x1b) {
                gpu_clear();

                printf("Exited\n");

                return;
            }

            int k = get_c8_key(data);

            if (k != -1) {
                c8_keydown(&c8, k);

                last_k = k;
            }
        } else {
            if (last_k != -1)
                c8_keyup(&c8, last_k);

            last_k = -1;
        }

        int ips = 60;

        while (ips--)
            c8_execute_instruction(&c8);

        gpu_clear();

        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                mmio_write_32(GPU_GP0, 0x70000000 | (c8.screen[x + (y * 64)] ? 0x0000AAFF : 0x000044AA));
                mmio_write_32(GPU_GP0, ((y * 8) << 16) | (x * 8));
            }
        }
    }

    return EXIT_SUCCESS;
}