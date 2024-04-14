#ifndef RTC_H
#define RTC_H

#include "libc/stdint.h"

#define RTC_TIMEL 0x9f900010
#define RTC_TIMEH 0x9f900014
#define RTC_CTRL  0x9f900018

struct rtc_time {
    union {
        uint64_t u64;
        uint32_t u32[2];
    };
};

struct rtc_time rtc_get_time(void);
uint32_t rtc_get_timel(void);
uint32_t rtc_get_timeh(void);
void rtc_discard(void);

#endif