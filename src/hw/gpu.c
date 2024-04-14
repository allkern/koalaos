#include "gpu.h"

#include "util/mmio.h"

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

void gpu_putchar(int c) {
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