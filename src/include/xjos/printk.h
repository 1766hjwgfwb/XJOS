#ifndef XJOS_PRINTK_H
#define XJOS_PRINTK_H

#include <libc/stdarg.h>



int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt,...);

int printk(const char *fmt,...);








#endif /* XJOS_PRINTK_H */