#ifndef XJOS_MUTEX_H
#define XJOS_MUTEX_H

#include <xjos/types.h>
#include <xjos/list.h>

/*
    mutex signal
*/

typedef struct {
    int8 value;
    list_t waiters; // wait queue
}semaphore_t;


void sem_init(semaphore_t *sem);
void sem_wait(semaphore_t *sem);    // P
void sem_post(semaphore_t *sem);    // V

/*
    mutex
*/

typedef struct {
    struct task_t *holder;
    semaphore_t sem;
    u32 repeat;                     // ref count
}mutex_t;

void mutex_init(mutex_t *lock);
void mutex_lock(mutex_t *lock);
void mutex_unlock(mutex_t *lock);

/*
    spinlock
*/



#endif /* XJOS_MUTEX_H */