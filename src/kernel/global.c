#include <xjos/global.h>
#include <libc/string.h>
#include <xjos/debug.h>


descriptor_t gdt[GDT_SIZE];
gdt_ptr_t gdt_ptr;


// Taking over the GDT table from the kernel
void gdt_init() {
    DEBUGK("init gdt!!!\n");

    asm volatile("sgdt gdt_ptr");

    // byte copy, gdt[0] 8B gdt[1] 8B... gdt[GDT_SIZE-1] 8B, (gdt_ptr.limit + 1)/8 = index
    memcpy(&gdt, (void *)gdt_ptr.base, gdt_ptr.limit + 1);

    gdt_ptr.base = (u32)&gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;


    asm volatile("lgdt gdt_ptr");
}