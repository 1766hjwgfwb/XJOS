extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void hang();
extern void time_init();
extern void rtc_init();
extern void memory_map_init();
extern void mapping_int();
extern void memory_test();
#include <xjos/types.h>
#include <xjos/interrupt.h>
#include <xjos/debug.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

void kernel_init() {
    interrupt_init();

    memory_map_init();
    mapping_int();

    // memory_test();
    bool state = interrupt_disable();
    set_interrupt_state(state);

    LOGK("%d\n", state);
    LOGK("%d\n", get_interrupt_state());

    BMB;
    

    hang();
}
