#ifndef XJOS_CONSOLE_H
#define XJOS_CONSOLE_H

#include <xjos/types.h>

#define CRT_ADDR_REG 0x3D4 // CRT(6845)
#define CRT_DATA_REG 0x3D5

#define CRT_START_ADDR_H 0xC // video memory start address - high byte
#define CRT_START_ADDR_L 0xD // video memory start address - low byte
#define CRT_CURSOR_H 0xE     // currsor position - high byte
#define CRT_CURSOR_L 0xF     // currsor position - low byte

#define MEM_BASE 0xB8000              // video memory start address
#define MEM_SIZE 0x4000               // video memory size
#define MEM_END (MEM_BASE + MEM_SIZE) // video memory end address
#define WIDTH 80                      // screen width
#define HEIGHT 25                     // screen height
#define ROW_SIZE (WIDTH * 2)          // bytes per row
#define SCR_SIZE (ROW_SIZE * HEIGHT)  // total screen size

#define NUL 0x00
#define ENQ 0x05
#define ESC 0x1B // ESC
#define BEL 0x07 // \a
#define BS 0x08  // \b
#define HT 0x09  // \t
#define LF 0x0A  // \n
#define VT 0x0B  // \v
#define FF 0x0C  // \f
#define CR 0x0D  // \r
#define DEL 0x7F

void console_init();
void console_clear();
int32 console_write(const char *buf, u32 count);









#endif /* XJOS_CONSOLE_H */