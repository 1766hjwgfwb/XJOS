#include <xjos/xjos.h>
#include <hardware/io.h>
#include <drivers/console.h>
#include <libc/printk.h>



void kernel_init() {
    console_init();

    printk("Test 1: decimal %d\n", 12345);
    printk("Test 2: negative %d\n", -12345);
    printk("Test 3: hex lower %x\n", 0x1a2b3c);
    printk("Test 4: hex upper %X\n", 0x1a2b3c);
    printk("Test 5: octal %o\n", 0755);
    printk("Test 6: pointer %p\n", (void *)0xCAFEBABE);
    printk("Test 7: string: %s\n", "hello kernel");
    printk("Test 8: char: %c %c %c\n", 'A', 'B', 'C');
    printk("Test 9: width %10d end\n", 42);
    printk("Test 10: zero-pad %010d end\n", 42);
    printk("Test 11: left align %-10d end\n", 42);
    printk("Test 12: plus %+d, space % d\n", 99, 99);
    printk("Test 13: special %#x, %#o\n", 0xdead, 0777);


    return;
}
