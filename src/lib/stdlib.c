#include <xjos/stdlib.h>



void delay(u32 count) {
    while(count--)
        asm volatile("nop");
}


void hang() {
    while(1);
}