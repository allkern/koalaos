#include "gpu.h"

#include "util/mmio.h"
#include "libc/ctype.h"
#include "libc/stdlib.h"
#include "hw/uart.h"

static int ansi = 0;
static char ansi_n[4];
static char ansi_m[4];
static char* ansi_n_p = ansi_n;
static char* ansi_m_p = ansi_m;
static int state = 0;

int __ffssi2(int a) {
    if (a == 0)
        return 0;

    return __builtin_ctz(a) + 1;
}

uint16_t get_char_uv(int c) {
    return ((c & 0xf0) << h_shift) | ((c & 0xf) << 3);
}

/*
GP0(80h) - Copy Rectangle (VRAM to VRAM)

  1st  Command           (Cc000000h)
  2nd  Source Coord      (YyyyXxxxh)  ;Xpos counted in halfwords
  3rd  Destination Coord (YyyyXxxxh)  ;Xpos counted in halfwords
  4th  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords
*/

void gpu_scroll_down(void) {
    mmio_write_32(GPU_GP0, 0x80000000);
    mmio_write_32(GPU_GP0, h);
    mmio_write_32(GPU_GP0, 0x00000000);
    mmio_write_32(GPU_GP0, (0x1e00000 - h) | 640);

    mmio_write_32(GPU_GP0, 0x02080808);
    mmio_write_32(GPU_GP0, 0x1e00000 - h);
    mmio_write_32(GPU_GP0, h | 640);
}

uint32_t gpu_get_xpos(void) {
    return xy & 0xffff;
}

void gpu_set_xpos(uint32_t x) {
    xy &= 0xffff0000;
    xy |= x;
}

uint32_t gpu_get_ypos(void) {
    return xy >> 16;
}

void gpu_set_ypos(uint32_t y) {
    xy &= 0xffff;
    xy |= y << 16;
}

uint32_t gpu_get_pos(void) {
    return xy;
}

void gpu_set_pos(uint32_t pos) {
    xy = pos;
}

void gpu_parse_ansi(char c) {
    if (c == 'm') {
        int n = atoi(ansi_n);

        uint8_t bg = attr & 0xf0;
        int at = -1;

        switch (n) {
            case 0: gpu_restore_attribute(); break;
            case 30: at = bg | 0; break;
            case 31: at = bg | 1; break;
            case 32: at = bg | 2; break;
            case 33: at = bg | 3; break;
            case 34: at = bg | 4; break;
            case 35: at = bg | 5; break;
            case 36: at = bg | 6; break;
            case 37: at = bg | 7; break;
            case 90: at = bg | 8; break;
            case 91: at = bg | 9; break;
            case 92: at = bg | 10; break;
            case 93: at = bg | 11; break;
            case 94: at = bg | 12; break;
            case 95: at = bg | 13; break;
            case 96: at = bg | 14; break;
            case 97: at = bg | 15; break;
        }

        if (*ansi_m) {
            int m = atoi(ansi_m);

            switch (m) {
                case 40: at = (at & 0xf) | 0x00; break;
                case 41: at = (at & 0xf) | 0x10; break;
                case 42: at = (at & 0xf) | 0x20; break;
                case 43: at = (at & 0xf) | 0x30; break;
                case 44: at = (at & 0xf) | 0x40; break;
                case 45: at = (at & 0xf) | 0x50; break;
                case 46: at = (at & 0xf) | 0x60; break;
                case 47: at = (at & 0xf) | 0x70; break;
                case 100: at = (at & 0xf) | 0x80; break;
                case 101: at = (at & 0xf) | 0x90; break;
                case 102: at = (at & 0xf) | 0xa0; break;
                case 103: at = (at & 0xf) | 0xb0; break;
                case 104: at = (at & 0xf) | 0xc0; break;
                case 105: at = (at & 0xf) | 0xd0; break;
                case 106: at = (at & 0xf) | 0xe0; break;
                case 107: at = (at & 0xf) | 0xf0; break;
            }
        }

        if (at != -1)
            gpu_set_attribute(at);

        state = 0;
        ansi = 0;
        *ansi_n = '\0';
        *ansi_m = '\0';
        ansi_n_p = ansi_n;
        ansi_m_p = ansi_m;
    }

    switch (state) {
        case 0:
            state = (c == '[') ? 1 : 0;
        break;
        case 1:
            if (c == ';') {
                state = 2;

                return;
            }

            *ansi_n_p++ = c;
            *ansi_n_p = '\0';
        break;
        case 2:
            *ansi_m_p++ = c;
            *ansi_m_p = '\0';
        break;
    }
}

void gpu_putchar(int c) {
    if (ansi) {
        gpu_parse_ansi(c);

        return;
    }

    if (c == '\n') {
        xy &= 0xffff0000;

        int pxy = xy;

        xy += h;

        if ((xy >> 16) == 480) {
            xy = 0;

            xy = pxy;

            gpu_scroll_down();
        }

        return;
    } else if (c == '\r') {
        xy &= 0xffff0000;

        return;
    // ANSI escape sequence
    } else if (c == '\x1b') {
        ansi = 1;
        state = 0;

        return;
    }

    uint16_t uv = get_char_uv(c);

    mmio_write_32(GPU_GP0, 0x65000000);
    mmio_write_32(GPU_GP0, xy);
    mmio_write_32(GPU_GP0, 0x78000000 | uv);

    mmio_write_32(GPU_GP0, 0x8 | h);

    xy += 8;
}

void gpu_update_clut(uint32_t* clut) {
    int b = attr & 0xf;
    int f = attr >> 4;

    g_clut[0] = clut_palette[f + (b * 0x10)];

    mmio_write_32(GPU_GP0, 0xa0000000);
    mmio_write_32(GPU_GP0, 0x01e00000);
    mmio_write_32(GPU_GP0, 0x00010010);

    for (int i = 0; i < 8; i++)
        mmio_write_32(GPU_GP0, clut[i]);
}

void gpu_set_attribute(uint8_t at) {
    prev_attr = attr;
    attr = at;

    gpu_update_clut(g_clut);
}

uint8_t gpu_get_attribute(void) {
    return attr;
}

void gpu_restore_attribute() {
    attr = prev_attr;

    gpu_update_clut(g_clut);
}

void gpu_clear() {
    uint32_t b = rgb555_palette[(attr >> 4) & 0xf];

    b = ((b & 0x001f) << 3) |
        ((b & 0x03e0) << 6) |
        ((b & 0x7c00) << 9);

    mmio_write_32(GPU_GP0, 0x02000000 | b);
    mmio_write_32(GPU_GP0, 0x00000000);
    mmio_write_32(GPU_GP0, 0x01e00280);

    xy = 0;
}

void gpu_init(const uint32_t* font, int height) {
    xy = 0;
    h = height << 16;
    h_shift = __builtin_ffs(height) + 3;
    attr = 0x07;
    prev_attr = attr;

    for (int i = 0; i < 4; i++) {
        ansi_n[i] = 0;
        ansi_m[i] = 0;
    }

    // Reset GPU
    mmio_write_32(GPU_GP1, 0x00000000);

    // Set resolution to 640x480, NTSC, interlaced
    mmio_write_32(GPU_GP1, 0x08000027);

    // Set drawing area to 0,0-640,480
    mmio_write_32(GPU_GP0, 0xe3000000);
    mmio_write_32(GPU_GP0, 0xe4078280);

    // Clear the screen (fast)
    gpu_clear();

    // Upload font to GPU
    mmio_write_32(GPU_GP0, 0xa0000000);
    mmio_write_32(GPU_GP0, 0x00000280);

    mmio_write_32(GPU_GP0, 0x20 | (h << 4));

    for (int i = 0; i < (h >> 8); i++)
        mmio_write_32(GPU_GP0, font[i]);

    gpu_update_clut(g_clut);

    // Set up texpage (x=640, y=0, enable drawing)
    mmio_write_32(GPU_GP0, 0xe100040a);
}

/*
  1st  Command
  2nd  Destination Coord (YyyyXxxxh)  ;Xpos counted in halfwords
  3rd  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords
  ...  Data              (...)      <--- usually transferred via DMA
*/
void gpu_upload_tex(uint16_t* buf, int x, int y, int width, int height) {
    mmio_write_32(GPU_GP0, 0xa0000000);
    mmio_write_32(GPU_GP0, (y << 16) | x);
    mmio_write_32(GPU_GP0, (height << 16) | width);

    uint32_t size = width * height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int yk = height - y;

            uint32_t l = buf[(x++) + (yk * width)];
            uint32_t h = buf[x + (yk * width)];

            l = ((l & 0x7c00) >> 10) | ((l & 0x001f) << 10) | (l & 0x03e0);
            h = ((h & 0x7c00) >> 10) | ((h & 0x001f) << 10) | (h & 0x03e0);

            mmio_write_32(GPU_GP0, l | (h << 16));
        }
    }
}