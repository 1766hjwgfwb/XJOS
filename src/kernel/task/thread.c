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
        printf("init thread %d %d %d ...\n", getpid(), getppid(), counter++);

        sleep(1000);
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
        printf("test thread %d %d %d ...\n", getpid(), getppid(), count++);
        sleep(2000);
    }
}