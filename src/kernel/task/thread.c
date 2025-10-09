#include <xjos/interrupt.h>
#include <xjos/syscall.h>
#include <xjos/debug.h>
#include <xjos/mutex.h>

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

mutex_t mutex;


void init_thread() {
    set_interrupt_state(true);
    u32 count = 0;

    mutex_init(&mutex);

    while (true) {
        mutex_lock(&mutex);
        LOGK("init task... %d\n", count++);
        sleep(10);
        mutex_unlock(&mutex);
    }
}


void test_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        mutex_lock(&mutex);
        LOGK("Test thread running %d\n", count++);
        sleep(10);
        mutex_unlock(&mutex);
    }
}