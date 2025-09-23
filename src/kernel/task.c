#include <xjos/task.h>
#include <xjos/printk.h>
#include <xjos/debug.h>

#define PAGE_SIZE 0x1000

task_t *a = (task_t *)0x1000;
task_t *b = (task_t *)0x2000;

extern void task_switch(task_t *next);

task_t *running_task() {
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n");    // clear page offset, return 0x1000 or 0x2000
}


void schedule() {
    task_t *current = running_task();
    task_t *next = current == a ? b : a;
    task_switch(next);
}


u32 _ofp thread_a() {
    asm volatile("sti\n");

    while (true) {
        printk("A");
    }
}


u32 _ofp thread_b() {
    asm volatile("sti\n");

    while (true) {
        printk("B");
    }
}


static void task_create(task_t *task, target_t target) {
    u32 stack = (u32)task + PAGE_SIZE;

    // * exp. 0x2000 - 0x14 = 0x1fec
    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->ebx = 0x11111111; // 0x1fec
    frame->esi = 0x22222222; // 0x1ff0
    frame->edi = 0x33333333; // 0x1ff4
    frame->ebp = 0x44444444; // 0x1ff8
    // * pop eip, so eip point -> target
    frame->eip = (void *)target;    // 0x1ffc

    task->stack = (u32 *)stack; // [0x1000] = 0x1fec
}


void task_init() {
    task_create(a, thread_a);
    task_create(b, thread_b);
    schedule();
}
