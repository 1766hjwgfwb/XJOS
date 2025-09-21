#ifndef XJOS_GLOBAL_H
#define XJOS_GLOBAL_H


#include <xjos/types.h>


#define GDT_SIZE 128    // max = 8192 ->  2 ^ 13 

void gdt_init();


// Global Descriptor Table (GDT)
typedef struct descriptor_struct {
    u16 limit_low;           // segment limit (bits 0-15)
    u32 base_low : 24;         // base address (bits 0-23)
    u8 type : 4;             // segment type
    u8 segment : 1;          // 0 = system segment, 1 = data/code segment
    u8 DPL : 2;              // descriptor privilege level 0 ~ 3
    u8 present : 1;          // present flag, memory 1, disk 0
    u8 limit_high : 4;       // segment limit (bits 16-19)
    u8 available : 1;        // ????
    u8 long_mode : 1;        // 64-bit code segment flag
    u8 big : 1;              // 32-bit code segment flag
    u8 granularity : 1;      // limit granularity flag (0 = 1B - 1MB, 1 = 1KB - 1GB)
    u8 base_high;            // base address (bits 24-31)
}_packed descriptor_t;


// segment selectors
typedef struct {
    u8 RPL : 2;
    u8 TI : 1;
    u16 index : 13;
}selector_t;


// GDT pointer
typedef struct {
    u16 limit;     // size of GDT - 1
    u32 base;
}_packed gdt_ptr_t;



#endif /* XJOS_GLOBAL_H */