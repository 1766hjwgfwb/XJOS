#include <xjos/debug.h>
#include <libc/stdarg.h>
#include <xjos/printk.h>
#include <libc/stdio.h>


static char buf[1024];


void debug(char *file, int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    va_end(args);

    printk("[%s] [%d] %s", file, line, buf);
}