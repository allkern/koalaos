#ifndef NVS_H
#define NVS_H

#include "libc/stdint.h"

#define NVS_SECTOR_SIZE 0x200
#define NVS_PHYS_BASE 0x9fa00000

enum {
    NVS_REG_DATA = 0x9fa00000,
    NVS_REG_LBA  = 0x9fa00004,
    NVS_REG_CMD  = 0x9fa00008,
    NVS_REG_STAT = 0x9fa0000c
};

enum {
    NVS_CMD_NOP           = 0x00,
    NVS_CMD_READ_SECTOR   = 0x01,
    NVS_CMD_READ_SECTORS  = 0x11,
    NVS_CMD_WRITE_SECTOR  = 0x02,
    NVS_CMD_WRITE_SECTORS = 0x12,
    NVS_CMD_IDENT         = 0x03
};

enum {
    NVS_STAT_IODREQ = 0x00000001,
    NVS_STAT_PROBE  = 0x40000000,
    NVS_STAT_BUSY   = 0x80000000
};

typedef struct {
    uint32_t type;
    char model[128];
    char manufacturer[32];
    uint32_t sector_count;
    uint32_t sector_size;
} nvs_id;

unsigned int nvs_get_status(int index);
void nvs_read_sector(int index, void* buf, uint32_t lba);
void nvs_write_sector(int index, void* buf, uint32_t lba);
void nvs_zero_sector(int index, uint32_t lba);
void nvs_read_ident(int index, void* buf);

#endif