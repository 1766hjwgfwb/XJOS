#include <xjos/interrupt.h>
#include <xjos/syscall.h>
#include <xjos/debug.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)


void idle_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        LOGK("Idle thread running %d\n", count++);
        asm volatile(
            "sti\n"
            "hlt\n"
        );
        yield();
    }
}


void init_thread() {
    set_interrupt_state(true);

    while (true) {
        LOGK("init task...\n");
        test();
    }
}