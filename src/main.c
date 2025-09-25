extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void hang();
extern void time_init();
extern void rtc_init();
extern void memory_map_init();
extern void memory_test();


void kernel_init() {
    // console_init();
    // gdt_init();

    memory_map_init();
    interrupt_init();

    memory_test();
    clock_init();
    // time_init();

    // rtc_init();

    asm volatile("sti\n");

    hang();
}
