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
    u32 base_priority;       // base priority
    u32 ticks;               // ticks to sleep
    u32 jiffies;             // global jiffies
    u32 age;                 // age
    char name[TASK_NAME_LEN]; // task name
    u32 uid;                 // user id
    pid_t pid;
    pid_t ppid;
    u32 pde;                 // page directory entry
    bitmap_t *vmap;          // process virtual memory bitmap
    u32 brk;                 // process heap top
    u32 magic;               // kernel magic number
}task_t;

typedef struct {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    void (*eip)(void);
}task_frame_t;

// interrupt frame
typedef struct {
    u32 vector;

    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp_dummy;      // not used

    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;

    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;

    u32 vector0;
    u32 error;

    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
}intr_frame_t;


void task_init();
task_t *running_task();
void schedule();

void task_yield();
void task_block(task_t *task, list_t *blist, task_state_t state);
void task_unblock(task_t *task);

void task_sleep(u32 ms);
void task_wakeup();

void task_to_user_mode(target_t target);

pid_t sys_getpid();
pid_t sys_getppid();

pid_t task_fork();


#endif /* _XJOS_TASK_H_ */