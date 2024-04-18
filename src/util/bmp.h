#ifndef BMP_H
#define BMP_H

#include "libc/stdint.h"

struct __attribute__((packed)) bmp_hdr {
    char h_signature[2];
    uint32_t h_file_size;
    uint32_t h_unused;
    uint32_t h_data_offset;
};

struct __attribute__((packed)) bmp_info {
    uint32_t i_size;
    uint32_t i_width;
    uint32_t i_height;
    uint16_t i_planes;
    uint16_t i_bpp;
    uint32_t i_compression;
    uint32_t i_image_size;
    uint32_t i_xpixelsperm;
    uint32_t i_ypixelsperm;
    uint32_t i_colors;
    uint32_t i_important;
};

struct bmp_s {
    struct bmp_hdr hdr;
    struct bmp_info info;
    void* buf;
};

enum {
    BMP_OK,
    BMP_NOFILE,
    BMP_INVALID,
    BMP_UNSUPPORTED
};

int bmp_open(struct bmp_s* bmp, const char* path);
void bmp_close(struct bmp_s* bmp);

#endif