#ifndef XJOS_RTC_H
#define XJOS_RTC_H

#include <xjos/types.h>


u8 cmos_read(u8 addr);
void cmos_write(u8 addr, u8 value);
void set_alarm(u32 secs);











#endif /* XJOS_RTC_H */