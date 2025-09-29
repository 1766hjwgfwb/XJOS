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
#include <xjos/debug.h>


void kernel_init() {
    // console_init();
    // gdt_init();
    interrupt_init();

    memory_map_init();
    mapping_int();

    BMB;
    console_init();
    char *ptr = ((void*)0);
    
    ptr[0] = 'a';
    BMB;
    // memory_test();

    hang();
}
