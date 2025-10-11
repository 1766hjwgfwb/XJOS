#include <xjos/interrupt.h>
#include <xjos/syscall.h>
#include <xjos/debug.h>
#include <xjos/mutex.h>
#include <xjos/spinlock.h>
#include <xjos/printk.h>

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

extern u32 keyboard_read(char *buf, u32 count);

void init_thread() {
    set_interrupt_state(true);
    u32 count = 0;

    char ch;

    while (true) {
        bool intr = get_interrupt_state();
        while (1) {
            keyboard_read(&ch, 1);
            printk("%c", ch);
        }
        set_interrupt_state(intr);
    }
}


void test_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        sleep(500);
    }
}