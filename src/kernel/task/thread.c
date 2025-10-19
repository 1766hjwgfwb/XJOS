#include <xjos/interrupt.h>
#include <xjos/syscall.h>
#include <xjos/debug.h>
#include <xjos/mutex.h>
#include <xjos/spinlock.h>
#include <libc/stdio.h>
#include <xjos/arena.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)


void idle_thread() {
    set_interrupt_state(true);
    while (true) {
        asm volatile(
            "sti\n"
            "hlt\n"
        );
        yield();
    }
}


static void user_init_thread() {
    u32 counter = 0;
    while (true) {
        BMB;
        char *ptr = (char *)0x900000;
        brk(ptr);
        BMB;
        
        ptr -= 0x1000;
        ptr[3] = 0xff;
        BMB;
        brk((char *)0x800000);
        BMB;


        sleep(1000000);
    }
}


void init_thread() {
    char temp[100];
    task_to_user_mode(user_init_thread);
}


void test_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        sleep(10000);
    }
}