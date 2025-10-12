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
#include <libc/string.h>
#include <xjos/global.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

#define NR_TASKS (64)
#define MAX_PRIORITY (32)
#define AGING_TIME (100)

extern u32 volatile jiffies;
extern u32 jiffy;
extern bitmap_t kernel_map;
extern void task_switch(task_t *next);
extern tss_t tss;

static task_t *tasks_table[NR_TASKS];   // task table
static task_t *idle_task;               // idle

static list_t block_list;               // blocked task list
static list_t sleep_list;               // sleep list

// ready list
static list_t ready_queues[MAX_PRIORITY];
static bitmap_t ready_bitmap;
static u8 ready_bitmap_bits[MAX_PRIORITY / 8];


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


void task_yield() {
    schedule();
}


// task stoppage
void task_block(task_t *task, list_t *blist, task_state_t state) {
    assert(!get_interrupt_state());
    assert(task->node.next == NULL && task->node.prev == NULL);
    assert(state != TASK_READY && state != TASK_RUNNING);

    if (blist == NULL)
        blist = &block_list;

    list_push(blist, &task->node);
    
    task->state = state;

    task_t *current = running_task();
    if (current == task)
        schedule();
}


void task_unblock(task_t *task) {
    assert(!get_interrupt_state());

    list_remove(&task->node);

    assert(task->node.next == NULL && task->node.prev == NULL);

    task->state = TASK_READY;

    assert(task->priority < MAX_PRIORITY);
    list_pushback(&ready_queues[task->priority], &task->node);
    bitmap_set(&ready_bitmap, task->priority, true);
}


void task_activate(task_t *task) {
    assert(task->magic == XJOS_MAGIC);

    if (task->uid != KERNEL_USER)
        tss.esp0 = (u32)task + PAGE_SIZE;
}


task_t *running_task() {
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n");    // clear page offset, return 0x1000 or 0x2000
}


void schedule() {
    assert(!get_interrupt_state());

    task_t *current = running_task();
    task_t *next = NULL;

    // Step 1: Put the current task (if it was running) back into the ready queue.
    if (current->state == TASK_RUNNING && current != idle_task) {
        current->state = TASK_READY;
        current->age = 0; // Reset age after running.
        // **Always put back to the base priority queue**
        list_pushback(&ready_queues[current->base_priority], &current->node);
        bitmap_set(&ready_bitmap, current->base_priority, true);
    }
    
    // Step 2: Find the next task, performing aging calculation at the same time.
    task_t *candidate = NULL;
    int max_score = -1;

    // Iterate through all ready tasks to find the one with the highest "score". O(N)
    for (int i = 0; i < MAX_PRIORITY; i++) {
        if (list_empty(&ready_queues[i])) 
            continue;
        
        list_node_t *ptr = ready_queues[i].head.next;
        while (ptr != &ready_queues[i].head) {
            task_t *task = list_entry(ptr, task_t, node);
            
            // **Increase the age for each waiting task (except for the 'current' one)**
            if (task != current) {
                task->age += 1;
            }
            
            // Calculate score = base priority + age weight.
            // AGING_TIME determines how much age equals one priority level.
            int score = task->base_priority + (task->age / AGING_TIME);
            
            if (score > max_score) {
                max_score = score;
                candidate = task;
            }
            ptr = ptr->next;
        }
    }

    // Step 3: Select the final candidate and pop it from its queue.
    if (candidate) {
        next = candidate;
        // **Only at this moment, do we perform the Pop (list_remove) operation**
        list_remove(&next->node); 
        if (list_empty(&ready_queues[next->base_priority])) {
            bitmap_set(&ready_bitmap, next->base_priority, false);
        }
    } else {
        next = idle_task;
    }

    // Step 4: Switch to the next task.
    assert(next != NULL);
    next->state = TASK_RUNNING;
    next->ticks = next->base_priority; // Timeslice is always based on the base priority.

    task_activate(next);
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

    task->stack = (u32 *)stack;         // esp pointer
    task->priority = priority;
    task->base_priority = priority;
    task->ticks = task->priority;      
    task->jiffies = 0;
    task->state = TASK_READY;
    task->uid = uid;
    task->vmap = &kernel_map;
    task->pde = KERNEL_PAGE_DIR;
    task->age = 0;
    task->magic = XJOS_MAGIC;       // canary 

    if (strcmp(task->name, "idle") != 0) {
        assert(task->priority < MAX_PRIORITY);
        list_pushback(&ready_queues[task->priority], &task->node);

        bitmap_set(&ready_bitmap, task->priority, true);
    }

    return task;
}


void task_to_user_mode(target_t target) {
    task_t *task = running_task();

    u32 addr = (u32)task + PAGE_SIZE;
    addr -= sizeof(intr_frame_t);
    intr_frame_t *iframe = (intr_frame_t *)addr;

    iframe->vector = 0x20;
    iframe->edi = 1;
    iframe->esi = 2;
    iframe->ebp = 3;
    iframe->esp_dummy = 4;
    iframe->ebx = 5;
    iframe->edx = 6;
    iframe->ecx = 7;
    iframe->eax = 8;

    iframe->gs = 0;
    iframe->ds = USER_DATA_SELECTOR;
    iframe->es = USER_DATA_SELECTOR;
    iframe->fs = USER_DATA_SELECTOR;
    iframe->ss = USER_DATA_SELECTOR;
    iframe->cs = USER_CODE_SELECTOR;

    iframe->error = XJOS_MAGIC;

    // alloc user stack
    u32 stack3 = alloc_kpage(1);

    iframe->eip = (u32)target;
    iframe->eflags = (0 << 12 | 0b10 | 1 << 9);
    iframe->esp = stack3 + PAGE_SIZE;

    // esp -> iframe
    asm volatile (
        "movl %0, %%esp\n"
        "jmp interrupt_exit\n" ::"m"(iframe)
    );
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
    
    for (int i = 0; i < MAX_PRIORITY; i++)
        list_init(&ready_queues[i]);

    task_setup();

    bitmap_init(&ready_bitmap, (char *)ready_bitmap_bits, MAX_PRIORITY, 0);


    idle_task = task_create(idle_thread, "idle", 1, KERNEL_USER);
    task_create(init_thread, "init", 5, NORMAL_USER);
    task_create(test_thread, "test", 3, KERNEL_USER);
}