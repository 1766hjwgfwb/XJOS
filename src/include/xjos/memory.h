#ifndef XJOS_MEMORY_H
#define XJOS_MEMORY_H

#include <xjos/types.h>

#define PAGE_SIZE 0x1000        // one page is 4KB
#define MEMORY_BASE 0x100000     // memory starts at 1M

// kernel memory size
#define KERNEL_MEMORY_SIZE 0x800000 // 8MB

// user stack top
#define USER_STACK_TOP 0x8000000    // 128MB

#define USER_STACK_SIZE 0x200000   // 2MB

// 128MB - 2MB = 126MB
#define USER_STACK_BOTTOM (USER_STACK_TOP - USER_STACK_SIZE)
 
#define KERNEL_PAGE_DIR 0x1000

typedef struct {
    u8 present : 1;             // in memroy or not
    u8 write : 1;               // 0 only read, 1 can write / read
    u8 user : 1;                // 0 kernel, 1 user
    u8 pwt : 1;                 // page write through 1/0
    u8 pcd : 1;                 // page cache disable 1/0
    u8 accessed : 1;            // has been accessed
    u8 dirty : 1;               // dirty page, has been written to
    u8 pat : 1;                 // page attribute table, 4K/4M
    u8 global : 1;              // global page, can be swapped
    u8 available : 3;           // reserved for OS use
    u32 index : 20;           // page index
}_packed page_entry_t;

u32 get_cr2();
u32 get_cr3();
void set_cr3(u32 pde);

// alloc and free count contiguous kernel pages
u32 alloc_kpage(u32 count);
void free_kpage(u32 vaddr, u32 count);

// vaddr <-> paddr
void link_page(u32 vaddr);
void unlink_page(u32 vaddr);

// copy pde
page_entry_t *copy_pde();

void free_pde();

// sys brk
int32 sys_brk(void *addr);

#endif /* XJOS_MEMORY_H */