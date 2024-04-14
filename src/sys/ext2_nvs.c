#include "hw/nvs.h"
#include "ext2.h"

#include "libc/string.h"
#include "libc/stdint.h"
#include "libc/stdio.h"

int impl_read_sector(void* buf, uint32_t lba) {
    nvs_read_sector(0, buf, lba);

    return 1;
}

int impl_write_sector(void* buf, uint32_t lba) {
    nvs_write_sector(0, buf, lba);

    return 1;
}