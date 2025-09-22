#ifndef XJOS_INTERRUPT_H
#define XJOS_INTERRUPT_H


#include <xjos/types.h>


#define IDT_SIZE 256

typedef struct {
    u16 offset0;        // segment offset bits 0..15
    u16 selector;       // segment selector
    u8 reserved;        // reserved
    u8 type : 4;        // task / interrupt / trap
    u8 segment : 1;     // segment = 0  system segment,  = 1  code or data segment
    u8 DPL: 2;          // descriptor privilige level
    u8 present : 1;     // present bit
    u16 offset1;        // segment offset bits 16..31
}_packed gate_t;


void interrupt_init();

typedef void* handler_t;



#endif /* XJOS_INTERRUPT_H */
