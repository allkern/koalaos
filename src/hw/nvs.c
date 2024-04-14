#include "nvs.h"

#include "util/mmio.h"
#include "libc/stdio.h"

#define NVSR(r) (r + (index * 16))

unsigned int nvs_get_status(int index) {
    return mmio_read_32(NVSR(NVS_REG_STAT));
}

void nvs_zero_sector(int index, uint32_t lba) {
    mmio_write_32(NVSR(NVS_REG_LBA), lba);
    mmio_write_32(NVSR(NVS_REG_CMD), NVS_CMD_WRITE_SECTOR);

    while (nvs_get_status(index) & NVS_STAT_IODREQ)
        mmio_write_32(NVSR(NVS_REG_DATA), 0);
}

void nvs_read_sector(int index, void* buf, uint32_t lba) {
    uint32_t* ptr = (uint32_t*)buf;

    mmio_write_32(NVSR(NVS_REG_LBA), lba);
    mmio_write_32(NVSR(NVS_REG_CMD), NVS_CMD_READ_SECTOR);

    while (nvs_get_status(index) & NVS_STAT_IODREQ)
        *ptr++ = mmio_read_32(NVSR(NVS_REG_DATA));
}

void nvs_write_sector(int index, void* buf, uint32_t lba) {
    uint32_t* ptr = (uint32_t*)buf;

    mmio_write_32(NVSR(NVS_REG_LBA), lba);
    mmio_write_32(NVSR(NVS_REG_CMD), NVS_CMD_WRITE_SECTOR);

    while (nvs_get_status(index) & NVS_STAT_IODREQ)
        mmio_write_32(NVSR(NVS_REG_DATA), *ptr++);
}

void nvs_read_ident(int index, void* buf) {
    uint32_t* ptr = (uint32_t*)buf;

    mmio_write_32(NVSR(NVS_REG_CMD), NVS_CMD_IDENT);

    while (nvs_get_status(index) & NVS_STAT_IODREQ)
        *ptr++ = mmio_read_32(NVSR(NVS_REG_DATA));
}