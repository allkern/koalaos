#include "hw/rtc.h"
#include "util/mmio.h"

struct rtc_time rtc_get_time(void) {
    struct rtc_time t;

    t.u32[0] = mmio_read_32(RTC_TIMEL);
    t.u32[1] = mmio_read_32(RTC_TIMEH);

    return t;
}

uint32_t rtc_get_timel(void) {
    uint32_t t = mmio_read_32(RTC_TIMEL);

    // Discard latch
    mmio_write_32(RTC_CTRL, 0);

    return t;
}

uint32_t rtc_get_timeh(void) {
    uint32_t t = mmio_read_32(RTC_TIMEH);

    // Discard latch
    mmio_write_32(RTC_CTRL, 0);

    return t;
}

void rtc_discard(void) {
    mmio_write_32(RTC_CTRL, 0);
}