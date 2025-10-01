#include <xjos/task.h>
#include <xjos/printk.h>
#include <xjos/debug.h>
#include <xjos/memory.h>
#include <libc/assert.h>
#include <xjos/interrupt.h>
#include <libc/string.h>
#include <xjos/bitmap.h>

extern bitmap_t kernel_map;
extern void task_switch(task_t *next);

#define NR_TASKS 64
static task_t *tasks_table[NR_TASKS];


static task_t *get_free_task() {
    for (int i = 0; i < NR_TASKS; i++) {
        if (tasks_table[i] == NULL) {
            tasks_table[i] = (task_t *)alloc_kpage(1);        // onec page for task_t
            return (task_t *)tasks_table[i];
        }
    }

    panic("No free task");
}


static task_t *task_search(task_state_t state) {
    assert(!get_interrupt_state());
    task_t *task = NULL;
    task_t *current = running_task();

    for (size_t i = 0; i < NR_TASKS; i++) {
        task_t *ptr = tasks_table[i];

        if (ptr == NULL)
            continue;
        
        if (ptr->state != state)
            continue;
        if (current == ptr)
            continue;
        if (task == NULL || task->ticks < ptr->ticks || ptr->jiffies < task->jiffies)
            task = ptr;
    }

    return task;
}


task_t *running_task() {
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n");    // clear page offset, return 0x1000 or 0x2000
}


void schedule() {
    task_t *current = running_task();
    task_t *next = task_search(TASK_READY);

    assert(next!= NULL);
    assert(next->magic == XJOS_MAGIC);

    if (current->state == TASK_RUNNING)
        current->state = TASK_READY;

    next->state = TASK_RUNNING;
    if (next == current)
        return;

    task_switch(next);
}


u32 _ofp thread_a() {
    set_interrupt_state(true);

    while (true) {
        printk("A");
    }
}


u32 _ofp thread_b() {
    set_interrupt_state(true);

    while (true) {
        printk("B");
    }
}


u32 _ofp thread_c() {
    set_interrupt_state(true);

    while (true) {
        printk("C");
    }
}


static task_t *task_create(target_t target, const char *name, u32 priority, u32 uid) {
    task_t *task = get_free_task();
    memset(task, 0, PAGE_SIZE);

    u32 stack = (u32)task + PAGE_SIZE;

    // // * exp. 0x2000 - 0x14 = 0x1fec
    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->ebx = 0x11111111; // 0x1fec
    frame->esi = 0x22222222; // 0x1ff0
    frame->edi = 0x33333333; // 0x1ff4
    frame->ebp = 0x44444444; // 0x1ff8
    // * pop eip, so eip point -> target
    frame->eip = (void *)target;    // 0x1ffc

    assert(strlen(name) < 16);
    strcpy((char*)task->name, name);

    task->stack = (u32 *)stack;
    task->priority = priority;
    task->ticks = task->priority;      
    task->jiffies = 0;
    task->state = TASK_READY;
    task->uid = uid;
    task->vmap = &kernel_map;
    task->pde = KERNEL_PAGE_DIR;
    task->magic = XJOS_MAGIC;       // canary 

    return task;
}


static void task_setup() {
    task_t *task = running_task();
    task->magic = XJOS_MAGIC;
    task->ticks = 1;

    memset(tasks_table, 0, sizeof(tasks_table));
}


void task_init() {
    task_setup();

    task_create(thread_a, "a", 500, KERNEL_USER);
    task_create(thread_b, "b", 50, KERNEL_USER);
    task_create(thread_c, "c", 5, KERNEL_USER);
}
