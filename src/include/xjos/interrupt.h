#ifndef XJOS_INTERRUPT_H
#define XJOS_INTERRUPT_H


#include <xjos/types.h>


#define IDT_SIZE 256

enum {
    // Hardware IRQ numbers
    IRQ_CLOCK       = 0,    // Programmable Interval Timer (PIT)
    IRQ_KEYBOARD    = 1,    // Keyboard controller
    IRQ_CASCADE     = 2,    // Cascade line for the slave PIC
    IRQ_SERIAL_2    = 3,    // Serial port 2 (COM2)
    IRQ_SERIAL_1    = 4,    // Serial port 1 (COM1)
    IRQ_PARALLEL_2  = 5,    // Parallel port 2 (LPT2)
    IRQ_FLOPPY      = 6,    // Floppy disk controller
    IRQ_PARALLEL_1  = 7,    // Parallel port 1 (LPT1)
    IRQ_RTC         = 8,    // Real-Time Clock
    IRQ_REDIRECT    = 9,    // Redirected IRQ2, often available for peripherals
    IRQ_MOUSE       = 12,   // PS/2 Mouse controller
    IRQ_MATH        = 13,   // Math co-processor (FPU)
    IRQ_HARDDISK    = 14,   // Primary ATA hard disk controller
    IRQ_HARDDISK2   = 15,   // Secondary ATA hard disk controller

    // Base interrupt vector numbers for the PICs
    IRQ_MASTER_NR   = 0x20, // Base vector for the master PIC (IRQs 0-7)
    IRQ_SLAVE_NR    = 0x28, // Base vector for the slave PIC (IRQs 8-15)
};




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



typedef void* handler_t;

void send_eoi(int vector);

// Set the handler for the given IRQ number.
void set_interrupt_handler(u32 irq, handler_t handler);
void set_interrupt_mask(u32 irq, bool enable);

#endif /* XJOS_INTERRUPT_H */
