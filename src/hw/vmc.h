#ifndef VMC_H
#define VMC_H

#include "libc/stdint.h"

#define VMC_EXIT    0x9f800000
#define VMC_PUTCHAR 0x9f800004
#define VMC_TIMEL   0x9f800008
#define VMC_TIMEH   0x9f80000c

typedef union {
    uint64_t u64;
    uint32_t u32[2];
} vmc_time_t;

void vmc_exit(int code);
void vmc_putchar(int c);
vmc_time_t vmc_get_time(void);

#endif