extern void interrupt_init();
extern void clock_init();
extern void time_init();
extern void rtc_init();
extern void memory_map_init();
extern void mapping_int();
extern void task_init();
extern void syscall_init();
extern void keyboard_init();
extern void tss_init();

#include <xjos/interrupt.h>

void kernel_init() {
    tss_init();
    memory_map_init();
    mapping_int();
    interrupt_init();
    clock_init();
    keyboard_init();

    task_init();
    syscall_init();

    set_interrupt_state(true);
}
