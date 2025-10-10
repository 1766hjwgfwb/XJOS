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

spinlock_t spin;

void init_thread() {
    set_interrupt_state(true);
    u32 count = 0;

    spin_init(&spin, "spinlock");

    while (true) {
        spin_lock(&spin);
        LOGK("init task... %d\n", count++);
        spin_unlock(&spin);
        // sleep(10);
    }
}


void test_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        spin_lock(&spin);
        LOGK("Test thread running %d\n", count++);
        spin_unlock(&spin);
        // sleep(10);
    }
}