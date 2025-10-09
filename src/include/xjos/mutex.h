#ifndef XJOS_MUTEX_H
#define XJOS_MUTEX_H

#include <xjos/types.h>
#include <xjos/list.h>

typedef struct {
    bool value;
    list_t waiters; // wait queue
}mutex_t;


void mutex_init(mutex_t *mutex);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);



#endif /* XJOS_MUTEX_H */