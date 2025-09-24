#include <xjos/time.h>
#include <xjos/debug.h>
#include <xjos/stdlib.h>
#include <hardware/io.h>
#include <xjos/rtc.h>


#define LOGK(fmt, args...) DEBUGK(fmt, ##args)


#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

#define CMOS_SECOND 0x00    // Seconds (0-59)
#define CMOS_MINUTE 0x02    // Minutes (0-59)
#define CMOS_HOUR   0x04    // Hours (0-23)
#define CMOS_WEEKDAY 0x06    // Day of week (1-7, 1=Sunday)
#define CMOS_DAY    0x07    // Day of month (1-31)
#define CMOS_MONTH  0x08    // Month (1-12)
#define CMOS_YEAR   0x09    // Year (0-99)
#define CMOS_CENTURY 0x32    // Century (0-99)
#define CMOS_NMI 0x80

#define MINUTE 60         // 1 minute in seconds
#define HOUR (60 * MINUTE) // 1 hour in seconds
#define DAY (24 * HOUR)    // 1 day in seconds
#define YEAR (365 * DAY)   // 1 year in seconds


// days per month in non-leap year
static int month[13] = {
    0,
    31,
    28,
    31,
    30,
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31
};


time_t startup_time;
int century;


int get_yday(tm *time) {
    int res = 0;

    // add days for previous months
    for (int i = 1; i < time->tm_mon; i++) {
        res += month[i];
    }

    // add days for current month
    res += time->tm_mday;

    
    // history 
    int year;
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;

    if ((year + 2) % 4 && time->tm_mon > 2) {
        res -= 1;
    }

    return res;
}


time_t mktime(tm *time) {
    time_t res;
    int year;

    // 20st -> 21st
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;

    // seconds from 1970
    res = YEAR * year;

    res += DAY * ((year + 1) / 4);  

    // Pre month
    res += month[time->tm_mon] * DAY;

    if (time->tm_mon > 2 && ((year + 2) % 4)) {
        res -= DAY;
    }

    // Current month
    res += DAY * (time->tm_mday - 1);

    // Current Hour
    res += HOUR * time->tm_hour;

    // Current Minute
    res += MINUTE * time->tm_min;

    res += time->tm_sec;

    return res;
}


/* u8 cmos_read(u8 addr) {
    // cmos_NMT 0x80 | 0xxx, atomic read
    outb(CMOS_ADDR, CMOS_NMI | addr);
    return inb(CMOS_DATA);
} */


void time_read_bcd(tm *time) {
    do
    {
        time->tm_sec = cmos_read(CMOS_SECOND);
        time->tm_min = cmos_read(CMOS_MINUTE);
        time->tm_hour = cmos_read(CMOS_HOUR);
        time->tm_mday = cmos_read(CMOS_DAY);
        time->tm_mon = cmos_read(CMOS_MONTH);
        time->tm_year = cmos_read(CMOS_YEAR);
        time->tm_wday = cmos_read(CMOS_WEEKDAY);
        // time->tm_yday = get_yday(time);
        century = cmos_read(CMOS_CENTURY);
    } while (time->tm_sec != cmos_read(CMOS_SECOND));
}


void time_read(tm *time) {
    time_read_bcd(time);    // time in BCD format

    time->tm_sec = bcd_to_bin(time->tm_sec);
    time->tm_min = bcd_to_bin(time->tm_min);
    time->tm_hour = bcd_to_bin(time->tm_hour);
    time->tm_mday = bcd_to_bin(time->tm_mday);
    time->tm_mon = bcd_to_bin(time->tm_mon);
    time->tm_year = bcd_to_bin(time->tm_year);
    time->tm_wday = bcd_to_bin(time->tm_wday);
    time->tm_yday = get_yday(time);
    time->tm_isdst = -1;

    century = bcd_to_bin(century);
}


void time_init() {
    tm time;
    time_read(&time);       // read CMOS time

    startup_time = mktime(&time);

    LOGK("startup time: %d%d-%02d-%02d %02d:%02d:%02d\n",
         century, 
         time.tm_year, 
         time.tm_mon, 
         time.tm_mday, 
         time.tm_hour, 
         time.tm_min, 
         time.tm_sec
        );

    // hang();
}