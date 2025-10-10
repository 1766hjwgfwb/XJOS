#ifndef XJOS_SPINLOCK_H
#define XJOS_SPINLOCK_H


#include <xjos/types.h>


typedef struct {
    volatile u8 locked;   // 0=unlocked, 1=locked
    
    // debugging
    const char *name;
    int holder_cpu;
}spinlock_t;


void spin_init(spinlock_t *lock, const char *name);
void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);


#endif /* XJOS_SPINLOCK_H */