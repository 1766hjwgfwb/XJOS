#include <xjos/task.h>
#include <xjos/printk.h>
#include <xjos/debug.h>
#include <xjos/memory.h>
#include <libc/assert.h>
#include <xjos/interrupt.h>
#include <libc/string.h>
#include <xjos/bitmap.h>
#include <xjos/syscall.h>
#include <xjos/list.h>

#define NR_TASKS 64

extern u32 volatile jiffies;
extern u32 jiffy;
extern bitmap_t kernel_map;
extern void task_switch(task_t *next);

static task_t *tasks_table[NR_TASKS];   // task table
static list_t block_list;               // blocked task list
static task_t *idle_task;               // idle
static list_t sleep_list;               // sleep list


void task_sleep(u32 ms) {
    assert(!get_interrupt_state());

    u32 ticks = ms / jiffy;             // jiffy 10ms
    ticks = ticks > 0 ? ticks : 1;      // at least 1 jiffy

    task_t *current = running_task();
    current->ticks = jiffies + ticks;   // need time to wakeup
    
    assert(current->node.next == NULL);
    assert(current->node.prev == NULL);

    // task insert sleep list
    bool inserted = false;
    task_t *task_curosr = NULL;


    list_for_each_entry(task_curosr, &sleep_list, node) {
        // find the first task with ticks > current->ticks
        if (task_curosr->ticks > current->ticks) {
            list_insert_before(&task_curosr->node, &current->node);
            inserted = true;
            break;
        }
    }

    // if not inserted, pushback to sleep list
    if (!inserted)
        list_pushback(&sleep_list, &current->node);
    
    current->state = TASK_SLEEPING;
    schedule();
}


void task_wakeup() {
    assert(!get_interrupt_state());
    
    list_node_t *ptr = sleep_list.head.next;
    list_node_t *next;

    while (ptr != &sleep_list.head) {
        next = ptr->next;

        task_t *task = element_entry(task_t, node, ptr);
        if (task->ticks > jiffies)      // timestamp
            break;

        // ticks <= jiffies
        task->ticks = 0;
        task_unblock(task);

        ptr = next;
    }
}


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

    if (task == NULL && state == TASK_READY)
        task = idle_task;

    return task;
}


void task_yield() {
    schedule();
}


// task stoppage
void task_block(task_t *task, list_t *blist, task_state_t state) {
    assert(!get_interrupt_state());
    assert(task->node.next == NULL);
    assert(task->node.prev == NULL);

    if (blist == NULL)
        blist = &block_list;

    list_push(blist, &task->node);
    
    assert(state != TASK_READY && state != TASK_RUNNING);
    task->state = state;

    task_t *current = running_task();
    if (current == task)
        schedule();
}


void task_unblock(task_t *task) {
    assert(!get_interrupt_state());

    list_remove(&task->node);

    assert(task->node.next == NULL);
    assert(task->node.prev == NULL);

    task->state = TASK_READY;
}


task_t *running_task() {
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n");    // clear page offset, return 0x1000 or 0x2000
}


void schedule() {
    assert(!get_interrupt_state());

    task_t *current = running_task();
    task_t *next = task_search(TASK_READY);

    assert(next!= NULL);
    assert(next->magic == XJOS_MAGIC);

    if (current->state == TASK_RUNNING)
        current->state = TASK_READY;

    if (current->ticks <= 0)
        current->ticks = current->priority;
    
    next->state = TASK_RUNNING;
    if (next == current)
        return;


    task_switch(next);
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


extern void idle_thread();
extern void init_thread();
extern void test_thread();

void task_init() {
    list_init(&block_list);
    list_init(&sleep_list);
    task_setup();

    idle_task = task_create(idle_thread, "idle", 1, KERNEL_USER);
    task_create(init_thread, "init", 5, KERNEL_USER);
    task_create(test_thread, "test", 5, KERNEL_USER);
}
