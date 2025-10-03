extern void interrupt_init();
extern void clock_init();
extern void time_init();
extern void rtc_init();
extern void memory_map_init();
extern void mapping_int();
extern void task_init();
extern void syscall_init();

#include <xjos/interrupt.h>
#include <xjos/memory.h>
#include <xjos/list.h>
#include <xjos/debug.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

void kernel_init() {
    memory_map_init();
    mapping_int();
    

    u32 count = 3;
    list_t holder;
    list_t *list = &holder;
    list_init(list);
    list_node_t *node;
    
    while (count--)
    {
        node = (list_node_t *)alloc_kpage(1);
        list_push(list, node);
    }

    list_node_t *ptr = (list_node_t*)0x102000;

    LOGK("true / false %d\n", list_search(list, ptr));

    BMB;

    while (!list_empty(list))
    {
        node = list_pop(list);
        free_kpage((u32)node, 1);
    }

    BMB;


   /*  count = 3;
    while (count--)
    {
        node = (list_node_t *)alloc_kpage(1);
        list_pushback(list, node);
    }

    LOGK("list size %ld\n", list_len(list));

    while (!list_empty(list))
    {
        node = list_popback(list);
        free_kpage((u32)node, 1);
    }

    node = (list_node_t *)alloc_kpage(1);
    list_pushback(list, node);

    LOGK("search node 0x%p --> %d\n", node, list_search(list, node));
    LOGK("search node 0x%p --> %d\n", 0, list_search(list, 0));

    list_remove(node);
    free_kpage((u32)node, 1); */

    /* interrupt_init();
    clock_init();
    
    task_init();
    
    syscall_init();
    
    set_interrupt_state(true); */
}
