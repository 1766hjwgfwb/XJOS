#ifndef XJOS_SYSCALL_H
#define XJOS_SYSCALL_H


#include <xjos/types.h>


typedef enum {
    SYS_NR_TEST,
    SYS_NR_EXIT = 1,
    SYS_NR_FORK = 2,
    SYS_NR_WRITE = 4,
    SYS_NR_WAITPID = 7,
    SYS_NR_TIME = 13,
    SYS_NR_GETPID = 20,
    SYS_NR_BRK = 45,
    SYS_NR_GETPPID = 64,
    SYS_NR_YIELD = 158,
    SYS_NR_SLEEP = 162
}syscall_t;


u32 test();
void yield();
void sleep(u32 ms);

pid_t waitpid(pid_t pid, int32 *status);

pid_t getpid();
pid_t getppid();

pid_t fork();

void exit(int status);

int32 brk(void *addr);

int32 write(fd_t fd, const char *buf, u32 len);

time_t time();


#endif /* XJOS_SYSCALL_H */