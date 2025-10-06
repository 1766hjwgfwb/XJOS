#ifndef XJOS_SYSCALL_H
#define XJOS_SYSCALL_H


#include <xjos/types.h>


typedef enum {
    SYS_NR_TEST,
    SYS_NR_SLEEP,
    SYS_NR_YIELD
}syscall_t;


u32 test();
void yield();
void sleep(u32 ms);




#endif /* XJOS_SYSCALL_H */