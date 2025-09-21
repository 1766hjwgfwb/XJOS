#include <xjos/kernel.h>
#include <drivers/console.h>
#include <libc/printk.h>
#include <xjos/global.h>
#include <xjos/task.h>


void kernel_init() {
    console_init();
    gdt_init();

    task_init();


    return;
}
