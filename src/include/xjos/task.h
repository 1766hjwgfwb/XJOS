#ifndef XJOS_TASK_H
#define XJOS_TASK_H

#include <xjos/types.h>
#include <xjos/bitmap.h>
#include <xjos/list.h>

#define KERNEL_USER 0
#define NORMAL_USER 1

#define TASK_NAME_LEN 16

typedef void target_t();

typedef enum {
    TASK_INIT,          // Initial state
    TASK_RUNNING,       // executing
    TASK_READY,         // ready to run
    TASK_BLOCKED,       // blockage
    TASK_SLEEPING,      // sleeping
    TASK_WAITING,       // waiting for a resource
    TASK_DIED,          // task has died
}task_state_t;

typedef struct {
    u32 *stack;              // kernel stack
    list_node_t node;        // task postpage node
    task_state_t state;      // state   
    u32 priority;            // priority
    u32 ticks;               // ticks to sleep
    u32 jiffies;             // global jiffies
    char name[TASK_NAME_LEN]; // task name
    u32 uid;                 // user id
    u32 pde;                 // page directory entry
    bitmap_t *vmap;          // process virtual memory bitmap
    u32 magic;               // kernel magic number
}task_t;

typedef struct {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    void (*eip)(void);
}task_frame_t;


void task_init();
task_t *running_task();
void schedule();

void task_yield();
void task_block(task_t *task, list_t *blist, task_state_t state);
void task_unblock(task_t *task);

void task_sleep(u32 ms);
void task_wakeup();


#endif /* _XJOS_TASK_H_ */