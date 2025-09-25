#include <xjos/types.h>
#include <xjos/debug.h>
#include <libc/assert.h>
#include <xjos/memory.h>
#include <xjos/stdlib.h>
#include <libc/string.h>


#define LOGK(fmt, args...) DEBUGK(fmt, ##args)


#define ZONE_VALID 1    // ards Valid zone
#define ZONE_RESERVED 2 // ards Reserved zone

// * 20 bits for page index, 12 bits for offset
#define IDX(addr) ((addr) >> 12)    // get addr the page index
#define PAGE(idx) ((idx) << 12)    // get page index the addr
#define ASSERT_PAGE(addr) assert((addr & 0xfff) == 0)   // once page start address


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


void memory_test() {
    LOGK("Memory test\n");
    u32 pages[10];

    LOGK("Get 10 pages\n");
    for (int i = 0; i < 10; i++) {
        pages[i] = get_page();
        LOGK("Page %d addr 0x%p\n", i, pages[i]);
    }

    LOGK("current free pages %d\n", free_pages);

    for (int i = 0; i < 10; i++) {
        put_page(pages[i]);
        LOGK("Put page %d addr 0x%p\n", i, pages[i]);
    }

    LOGK("current free pages %d\n", free_pages);
}