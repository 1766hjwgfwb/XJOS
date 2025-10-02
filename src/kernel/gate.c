#include <xjos/interrupt.h>
#include <libc/assert.h>
#include <xjos/debug.h>
#include <xjos/syscall.h>


#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

#define SYSTEM_SIZE (64)

handler_t syscall_table[SYSTEM_SIZE];


void syscall_check(u32 nr) {
    if (nr >= SYSTEM_SIZE)
        panic("Invalid system call number %d", nr);
}


static void sys_default() {
    panic("Ssycall not implemented");
}


static u32 sys_test() {
    LOGK("System call test called");
    return 255;
}

extern void task_yield();

void syscall_init() {
    for (size_t i = 0; i < SYSTEM_SIZE; i++) {
        syscall_table[i] = sys_default;
    }

    syscall_table[SYS_NR_TEST] = sys_test;
    syscall_table[SYS_NR_YIELD] = task_yield;
}