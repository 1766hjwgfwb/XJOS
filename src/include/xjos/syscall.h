#ifndef XJOS_SYSCALL_H
#define XJOS_SYSCALL_H


#include <xjos/types.h>


typedef enum {
    SYS_NR_TEST,
    SYS_NR_YIELD
}syscall_t;


u32 test();
void yield();




#endif /* XJOS_SYSCALL_H */