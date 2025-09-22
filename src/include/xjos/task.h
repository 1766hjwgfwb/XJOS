#ifndef XJOS_TASK_H
#define XJOS_TASK_H
#include <xjos/types.h>



typedef u32 target_t();


typedef struct task_t {
    u32 *stack; 

}task_t;

typedef struct {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    void (*eip)(void);
}task_frame_t;


void task_init();


#endif /* _XJOS_TASK_H_ */