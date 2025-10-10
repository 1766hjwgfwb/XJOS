#ifndef XJOS_MUTEX_H
#define XJOS_MUTEX_H

#include <xjos/types.h>
#include <xjos/list.h>

/*
    sem
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


#endif /* XJOS_MUTEX_H */