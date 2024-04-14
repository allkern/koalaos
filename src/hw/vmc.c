#include "vmc.h"
#include "util/mmio.h"

void vmc_exit(int code) {
    mmio_write_32(VMC_EXIT, code);
}

void vmc_putchar(int c) {
    mmio_write_32(VMC_PUTCHAR, c);
}

vmc_time_t vmc_get_time(void) {
    vmc_time_t time;

    time.u32[0] = mmio_read_32(VMC_TIMEL);
    time.u32[1] = mmio_read_32(VMC_TIMEH);

    return time;
}