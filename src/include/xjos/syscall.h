#ifndef XJOS_SYSCALL_H
#define XJOS_SYSCALL_H


#include <xjos/types.h>


typedef enum {
    SYS_NR_TEST,
    SYS_NR_WRITE = 4,
    SYS_NR_BRK = 45,
    SYS_NR_SLEEP = 162,
    SYS_NR_YIELD = 158
}syscall_t;


u32 test();
void yield();
void sleep(u32 ms);

int32 brk(void *addr);

int32 write(fd_t fd, const char *buf, u32 len);




#endif /* XJOS_SYSCALL_H */