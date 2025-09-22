#include <xjos/kernel.h>
#include <drivers/console.h>
#include <xjos/printk.h>
#include <xjos/global.h>
#include <xjos/task.h>
#include <xjos/interrupt.h>
#include <xjos/debug.h>
#include <xjos/stdlib.h>


void kernel_init() {
    console_init();
    gdt_init();

    interrupt_init();
    
    asm volatile(
        "sti\n"
        "movl %eax, %eax\n"
    );


    u32 count = 0;

    while(1) {
        DEBUGK("looping in kernel init %d\n", count++);
        delay(10000000);
    }

    return;
}
