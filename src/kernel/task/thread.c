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

semaphore_t sem;

void init_thread() {
    set_interrupt_state(true);
    u32 count = 0;

    sem_init(&sem);

    while (true) {
        sem_wait(&sem);
        LOGK("init task... %d\n", count++);
        // sleep(10);
        sem_post(&sem);
    }
}


void test_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        sem_wait(&sem);
        LOGK("Test thread running %d\n", count++);
        // sleep(10);
        sem_post(&sem);
    }
}