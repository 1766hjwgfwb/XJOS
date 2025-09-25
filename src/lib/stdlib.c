#include <xjos/stdlib.h>



void delay(u32 count) {
    while(count--)
        asm volatile("nop");
}


void hang() {
    while(1);
}


// exp.   25 -> (00011001), but bcd -> (00100101)
u8 bcd_to_bin(u8 value) {
    return (value & 0x0f) + (value >> 4) * 10;
}


u8 bin_to_bcd(u8 value) {
    return (value / 10) * 0x10 + (value % 10);
}


// num / szie copies
u32 div_round_up(u32 num, u32 size) {
    return (num + size - 1) / size;
}