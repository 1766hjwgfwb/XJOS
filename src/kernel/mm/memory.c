#include <xjos/types.h>
#include <xjos/debug.h>
#include <libc/assert.h>
#include <xjos/memory.h>
#include <xjos/stdlib.h>
#include <libc/string.h>


#define LOGK(fmt, args...) DEBUGK(fmt, ##args)


#define ZONE_VALID 1    // ards Valid zone
#define ZONE_RESERVED 2 // ards Reserved zone

/*  
    directory page index (31 ~ 22)bits
    page table index (21 ~ 12)bits
    page offset (11 ~ 0)bits
*/
#define DIDX(addr) (((u32)addr >> 22) & 0x3ff)          // get addr pde indx 
#define TIDX(addr) (((u32)addr >> 12) & 0x3ff)          // get addr pte indx 
#define IDX(addr) ((u32)(addr) >> 12)                   //get addr page index 
#define PAGE(idx) ((u32)(idx) << 12)                    // page start address
#define ASSERT_PAGE(addr) assert((addr & 0xfff) == 0)   // once page start address


// kernel page directory
#define KERNEL_PAGE_DIR 0x1000

// kernel page table
static u32 KERNEL_PAGE_TABLE[] = {
    0x2000,
    0x3000
};

// (table / sizeof(u32)) * 4M = 8M
#define KERNEL_MEMORY_SIZE (0x100000 * sizeof(KERNEL_PAGE_TABLE))


typedef struct {
    u64 base;   // memory base
    u64 size;   // memory length
    u32 type;   // memory type
}_packed ards_t;


static u32 memory_base = 0;    // Available memory base, value should = 1M
static u32 memory_size = 0;    // Available memory size
static u32 total_pages = 0;    // all memory pages
static u32 free_pages = 0;     // free memory pages

#define used_pages (total_pages - free_pages)   // used memory pages


void memory_init(u32 magic, u32 addr) {
    u32 count;

    ards_t *ptr;

    // check magic number
    if (magic == XJOS_MAGIC) {
        count = *(u32*)addr;    // 4 bytes count
        ptr = (ards_t*)(addr + 4);  // +4 bytes pointer to ards_t array

        for (int i = 0; i < count; i++, ptr++) {
            LOGK("Memory base 0x%p size 0x%p type %d\n",
                (u32)ptr->base, (u32)ptr->size, (u32)ptr->type);
            // find max valid memory zone
            if (ptr->type == ZONE_VALID && ptr->size > memory_size) {
                memory_base = (u32)ptr->base;
                memory_size = (u32)ptr->size;
            }
        }
    } else {
        panic("Memory init failed: invalid magic number, 0x%p\n", (u32)magic);
    }

    LOGK("ARDS count %d\n", count);
    LOGK("Memory base 0x%p size 0x%p\n", (u32)memory_base, (u32)memory_size);

    assert(memory_base == MEMORY_BASE);
    assert((memory_size & 0xfff) == 0);

    // calculate total and free pages
    total_pages = IDX(memory_size) + IDX(MEMORY_BASE);  // 31M + 1M
    free_pages = IDX(memory_size);

    LOGK("Total pages %d Free pages %d\n", total_pages, free_pages);

    // check system memory size
    if (memory_size < KERNEL_MEMORY_SIZE) {
        panic("System memory is %dM too small, at least %dM needed\n", 
            memory_size / MEMORY_BASE, KERNEL_MEMORY_SIZE / MEMORY_BASE);
    }
}


static u32 start_page = 0;      // free start page index
static u8 *memory_map;          // pyhsical memory map
static u32 memory_map_pages = 0;    // physical memory used pages


void memory_map_init() {

    // init pyhsical memory map, 0x100000
    memory_map = (u8*)memory_base;

    /*
        why need div_round_up?
        Since we need to store the status of each page using one byte, 
        such as 0x00, 0x01, etc., 
        and we have to manage a total of total_pages (8162) pages, 
        we need to use `pages` to store them. 
        8162 divided by 4096 and rounded up gives us 2 pages.
    */
    memory_map_pages = div_round_up(total_pages, PAGE_SIZE);

    LOGK("Memory map page count %d\n", memory_map_pages);
    
    free_pages -= memory_map_pages;

    // clear pyhsical memory map
    memset((void*)memory_base, 0, memory_map_pages * PAGE_SIZE);

    // set start page index
    start_page = IDX(memory_base) + memory_map_pages;
    LOGK("Start page index %d\n", start_page);

    for (size_t i = 0; i < start_page; i++) {
        memory_map[i] = 1;  // set all pages as used
    }

    LOGK("Total pages %d free pages %d\n", total_pages, free_pages);
}


// distribute a page memory
static u32 get_page() {
    for (size_t i = start_page; i < total_pages; i++) {
        // find a free page
        if (!memory_map[i]) {
            memory_map[i] = 1;  // set as used
            free_pages--;

            LOGK("Get page index %d\n", i);
            assert(free_pages >= 0);

            u32 page = (u32)(i << 12);  // current page address
            LOGK("Get page addr 0x%p\n", page);

            return page;
        }
    }

    panic("No free page available\n");
}


// free a page memory
static void put_page(u32 addr) {
    ASSERT_PAGE(addr);      // page start address

    u32 idx = IDX(addr);
    // idx > 1M, and < total_pages
    assert(idx >= start_page && idx < total_pages);

    assert(memory_map[idx] >= 1);

    // pyhsical refer -1
    memory_map[idx]--;
    if (!memory_map[idx]) {
        free_pages++;
    }

    assert(free_pages > 0 && free_pages < total_pages);
    LOGK("Put page addr 0x%p\n", addr);
}


// get cr3 register value, page directory base address
u32 _inline get_cr3() {
    asm volatile("movl %cr3, %eax\n");
    // store eax, return cr3 value
}


// arg pde is the page directory entry address
_inline void set_cr3(u32 pde) {
    ASSERT_PAGE(pde);
    asm volatile("movl %%eax, %%cr3\n" ::"a"(pde));
}


// set cr3 reg, PE -> 1, enable paging
static _inline void enable_page() {
    asm volatile(
        "movl %cr0, %eax\n"
        "orl $0x80000000, %eax\n"
        "movl %eax, %cr0\n"
    );
}


static void entry_init(page_entry_t *entry, u32 index) {
    *(u32 *)entry = 0;

    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
}


static page_entry_t *get_pde() {
    // get pde[1023] -> pte[1023] -> page directory
    return (page_entry_t *)(0xfffff000);
}


static page_entry_t *get_pte(u32 vaddr) {
    // ffc00000 | 1023 1 0 pde[1023] -> pte[1](pde[1023])
    // (0x1000 + 1 * 4)
    return (page_entry_t *)(0xffc00000 | (DIDX(vaddr) << 12));
}


static void flush_tlb(u32 vaddr) {
    asm volatile("invlpg (%0)" :: "r"(vaddr) : "memory");
}


void mapping_int() {
/*   pyhsical 0x201000 (Page Table)
     +-----------------------------------------------------------------+
     | PTE[0] | PTE[1] | PTE[2] | ... | PTE[1023] |
     +-----------------------------------------------------------------+
     ^        ^        ^
     |        |        |
     0x201000 0x201004 0x201008

     (Physical Page Frames)
     +--------+--------+--------+-----+-----------+
     | PFN 0  | PFN 1  | PFN 2  | ... | PFN 1023  |
     +------------------------------------------------------------------+
     ^        ^        ^               ^
     |        |        |               |
     0x0000   0x1000   0x2000          0x3FF000
*/
    page_entry_t *pde = (page_entry_t*)KERNEL_PAGE_DIR;
    memset(pde, 0, PAGE_SIZE);

    idx_t index = 0;
    // page directory init
    for (idx_t didx = 0; didx < (sizeof(KERNEL_PAGE_TABLE) / 4); didx++) {
        page_entry_t *pte = (page_entry_t*)KERNEL_PAGE_TABLE[didx];
        memset(pte, 0, PAGE_SIZE);

        page_entry_t *entry = &pde[didx];   // pde -> pte
        entry_init(entry, IDX((u32)pte));


        for (idx_t tidx = 0; tidx < 1024; tidx++, index++) {
            // dont mapping the first page, *(null)
            if (index == 0)
                continue;
            page_entry_t *tentry = &pte[tidx];
            entry_init(tentry, index);
            memory_map[index] = 1;
        }
    }

    // pde[1023] -> page directory, we can access all page table
    page_entry_t *entry = &pde[1023];
    entry_init(entry, IDX(KERNEL_PAGE_DIR));

    set_cr3((u32)pde);

    enable_page((u32)pde);
}


void memory_test() {
    u32 vaddr = 0x4000000;
    u32 paddr = 0x1400000;
    u32 table = 0x900000;

    page_entry_t *pde = get_pde();
    LOGK("Page addr 0x%p\n", (u32)pde);

    page_entry_t *dentry = &pde[DIDX(vaddr)];
    entry_init(dentry, IDX(table));

    BMB;

    page_entry_t *pte = get_pte(vaddr);
    LOGK("Page addr 0x%p\n", (u32)pte);

    page_entry_t *tentry = &pte[TIDX(vaddr)];
    LOGK("tentry %p\n", (u32)tentry);
    entry_init(tentry, IDX(paddr));


    BMB;

    char *ptr = (char *)(0x4000000);
    *ptr = 'a';

    BMB;

    entry_init(tentry, IDX(0x1500000));
    flush_tlb(vaddr);

    BMB;

    ptr[2] = 'b';

    BMB;
}