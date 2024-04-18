#include "libc/stdio.h"

#include "usr/shell.h"
#include "sys/ext2.h"
#include "hw/gpu.h"

#include "bmp.h"

int bmp_open(struct bmp_s* bmp, const char* path) {
    struct ext2_fd file;

    if (ext2_fopen(&file, path, "rb")) {
        printf("Couldn't find path \'%s\'\n", path);

        return BMP_NOFILE;
    }

    ext2_fread(&file, &bmp->hdr, sizeof(struct bmp_hdr));
    ext2_fread(&file, &bmp->info, sizeof(struct bmp_info));
    
    if (bmp->hdr.h_signature[0] != 'B' || bmp->hdr.h_signature[1] != 'M')
        return BMP_INVALID;

    if (bmp->info.i_compression)
        return BMP_UNSUPPORTED;

    uint32_t bpp = (bmp->info.i_bpp >> 3);
    uint32_t size = (bmp->info.i_width * bmp->info.i_height) * bpp;
    uint16_t* buf = malloc(size);

    printf("reading image from disk... ");

    ext2_fread(&file, buf, size);

    printf("done\nuploading texture...\n");

    gpu_upload_tex(buf, 0, 0, bmp->info.i_width, bmp->info.i_height);

    ext2_fclose(&file);

    free(buf);

    return BMP_OK;
}

void bmp_close(struct bmp_s* bmp);