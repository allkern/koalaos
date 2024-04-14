// Editor note: Locale support in C is disgusting

#include "libc/time.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/stdio.h"

// Hardware support
#include "hw/rtc.h"

// idk
#include "tzfile.h"

// time_impl.h
const char* __utc = "UTC";

/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400*(31+29))

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)

char* __secs_to_hrt(time_t t) {
    // Save the time in Human
    // readable format
	static char buf[32];
 
    // Number of days in month
    // in normal year
    int daysOfMonth[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31 };
 
    long int currYear, daysTillNow, extraTime, extraDays,
        index, date, month, hours, minutes, seconds,
        flag = 0;
 
    // Calculate total days unix time T
    daysTillNow = t / (24 * 60 * 60);
    extraTime = t % (24 * 60 * 60);
    currYear = 1970;
 
    // Calculating current year
    while (1) {
        if (currYear % 400 == 0
            || (currYear % 4 == 0 && currYear % 100 != 0)) {
            if (daysTillNow < 366) {
                break;
            }
            daysTillNow -= 366;
        }
        else {
            if (daysTillNow < 365) {
                break;
            }
            daysTillNow -= 365;
        }
        currYear += 1;
    }
    // Updating extradays because it
    // will give days till previous day
    // and we have include current day
    extraDays = daysTillNow + 1;
 
    if (currYear % 400 == 0
        || (currYear % 4 == 0 && currYear % 100 != 0))
        flag = 1;
 
    // Calculating MONTH and DATE
    month = 0, index = 0;
    if (flag == 1) {
        while (1) {
            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            }
            else {
                if (extraDays - daysOfMonth[index] < 0) {
                    break;
                }
                month += 1;
                extraDays -= daysOfMonth[index];
            }
            index += 1;
        }
    }
    else {
        while (1) {
            if (extraDays - daysOfMonth[index] < 0) {
                break;
            }
            month += 1;
            extraDays -= daysOfMonth[index];
            index += 1;
        }
    }
 
    // Current Month
    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    }
    else {
        if (month == 2 && flag == 1)
            date = 29;
        else {
            date = daysOfMonth[month - 1];
        }
    }
 
    // Calculating HH:MM:YYYY
    hours = extraTime / 3600;
    minutes = (extraTime % 3600) / 60;
    seconds = (extraTime % 3600) % 60;

	sprintf(buf, "%d/%d/%4d %02u:%02u:%02u",
		date,
		month,
		currYear,
		hours,
		minutes,
		seconds
	);
 
    // Return the time
    return buf;
}

int __secs_to_tm(long long t, struct tm *tm)
{
	long long days, secs, years;
	int remdays, remsecs, remyears;
	int qc_cycles, c_cycles, q_cycles;
	int months;
	int wday, yday, leap;
	static const char days_in_month[] = {31,30,31,30,31,31,30,31,30,31,31,29};

	/* Reject time_t values whose year would overflow int */
	if (t < INT_MIN * 31622400LL || t > INT_MAX * 31622400LL)
		return -1;

	secs = t - LEAPOCH;
	days = secs / 86400;
	remsecs = secs % 86400;
	if (remsecs < 0) {
		remsecs += 86400;
		days--;
	}

	wday = (3+days)%7;
	if (wday < 0) wday += 7;

	qc_cycles = days / DAYS_PER_400Y;
	remdays = days % DAYS_PER_400Y;
	if (remdays < 0) {
		remdays += DAYS_PER_400Y;
		qc_cycles--;
	}

	c_cycles = remdays / DAYS_PER_100Y;
	if (c_cycles == 4) c_cycles--;
	remdays -= c_cycles * DAYS_PER_100Y;

	q_cycles = remdays / DAYS_PER_4Y;
	if (q_cycles == 25) q_cycles--;
	remdays -= q_cycles * DAYS_PER_4Y;

	remyears = remdays / 365;
	if (remyears == 4) remyears--;
	remdays -= remyears * 365;

	leap = !remyears && (q_cycles || !c_cycles);
	yday = remdays + 31 + 28 + leap;
	if (yday >= 365+leap) yday -= 365+leap;

	years = remyears + 4*q_cycles + 100*c_cycles + 400LL*qc_cycles;

	for (months=0; days_in_month[months] <= remdays; months++)
		remdays -= days_in_month[months];

	if (months >= 10) {
		months -= 12;
		years++;
	}

	if (years+100 > INT_MAX || years+100 < INT_MIN)
		return -1;

	tm->tm_year = years + 100;
	tm->tm_mon = months + 2;
	tm->tm_mday = remdays + 1;
	tm->tm_wday = wday;
	tm->tm_yday = yday;

	tm->tm_hour = remsecs / 3600;
	tm->tm_min = remsecs / 60 % 60;
	tm->tm_sec = remsecs % 60;

	return 0;
}

clock_t clock (void) {
    // To-do: Return COP0 Count

    return 0;
}

time_t time(time_t *t)
{
	time_t temp = rtc_get_timel();

	if (t)
		*t = temp;

	return temp;
}

// No FP support
// double difftime(time_t t1, time_t t0)

time_t difftime(time_t t1, time_t t0)
{
	return t1-t0;
}

size_t strftime (char *__restrict, size_t, const char *__restrict, const struct tm *__restrict);

struct tm *gmtime(const time_t *t)
{
	static struct tm tm;
	if (__secs_to_tm(*t, &tm) < 0) {
		errno = EOVERFLOW;
		return 0;
	}
	tm.tm_isdst = 0;
	tm.__tm_gmtoff = 0;
	tm.__tm_zone = __utc;

	return &tm;
}

struct tm *localtime (const time_t *);

char * asctime_r(const struct tm * tm, char * result) {
	static const char wday_name[][3] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char mon_name[][3] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	/*
	** Big enough for something such as
	** ??? ???-2147483648 -2147483648:-2147483648:-2147483648 -2147483648\n
	** (two three-character abbreviations, five strings denoting integers,
	** three explicit spaces, two explicit colons, a newline,
	** and a trailing ASCII nul).
	*/
	register const char * wn;
	register const char * mn;

	if (tm->tm_wday < 0 || tm->tm_wday >= DAYSPERWEEK)
		wn = "???";
	else wn = wday_name[tm->tm_wday];
	if (tm->tm_mon < 0 || tm->tm_mon >= MONSPERYEAR)
		mn = "???";
	else mn = mon_name[tm->tm_mon];
	/*
	** The X3J11-suggested format is
	**	"%.3s %.3s%3d %02.2d:%02.2d:%02.2d %d\n"
	** Since the .2 in 02.2d is ignored, we drop it.
	*/
	(void) sprintf(result, "%.3s %.3s %d %02d:%02d:%02d %d",
		wn, mn,
		tm->tm_mday, tm->tm_hour,
		tm->tm_min, tm->tm_sec,
		TM_YEAR_BASE + tm->tm_year);

	return result;
}
char *asctime(const struct tm *tm)
{
	static char buf[26];

	asctime_r(tm, buf);

	return buf;
}

char *ctime (const time_t *);
int timespec_get(struct timespec *, int);

#define CLOCKS_PER_SEC 1000000L