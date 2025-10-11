#include <xjos/interrupt.h>
#include <xjos/syscall.h>
#include <xjos/debug.h>
#include <xjos/mutex.h>
#include <xjos/spinlock.h>

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


void init_thread() {
    set_interrupt_state(true);
    u32 count = 0;


    while (true) {
        sleep(500);
    }
}


void test_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        sleep(500);
    }
}