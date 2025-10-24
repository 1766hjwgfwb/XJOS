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
    int status;
    while (true) {

        pid_t x = fork();
        if (x) {
            printf("fork after parent %d, %d, %d\n", x, getpid(), getppid());
            // sleep(1000);
            pid_t child = waitpid(x, &status);
            printf("waitpid %d, %d\n", child, status);
        } else {
            printf("fork after child %d, %d, %d\n", x, getpid(), getppid());
            sleep(1000);
            exit(5);
        }
        while(1);
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
        sleep(2000);
    }
}