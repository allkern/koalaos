#include "libc/string.h"
#include "libc/stdio.h"
#include "libc/time.h"
#include "libc/tzfile.h"

#include "hw/rtc.h"

int usr_time(int argc, const char* argv[]) {
    time_t t = time(NULL);

    const char* zone = NULL;
    const char* fmt = NULL;

    // for (int i = 1; i < argc; i++) {
    //     if (strlen(argv[i]) != 2)
    //         continue;

    //     if (!strcmp(argv[i], '-f')) fmt = argv[i+i];
    //     if (!strcmp(argv[i], '-z')) zone = argv[i+i];
    // }

    // return EXIT_SUCCESS;

    int tz_utchoff = 0;

    if (zone) {
        if (!strncmp(zone, "AR", 2)) tz_utchoff = -3;
    }

    t += tz_utchoff * SECS_PER_HOUR;

    struct tm* tm = gmtime(&t);

    // printf("%s (%s)\n", asctime(tm), __secs_to_hrt(t));

    if (!fmt)
        fmt = "%a %b %e %H:%M:%S %Y (%e/%m/%Y)";

    char buf[128];

    if (strftime(buf, 128, fmt, tm))
        puts(buf);
    else
        puts("strftime failed");

    return EXIT_SUCCESS;
}