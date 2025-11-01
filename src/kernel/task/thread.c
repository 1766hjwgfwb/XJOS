#include <xjos/interrupt.h>
#include <xjos/syscall.h>
#include <xjos/debug.h>
#include <xjos/mutex.h>
#include <xjos/spinlock.h>
#include <libc/stdio.h>
#include <xjos/arena.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)


void idle_thread() {
    set_interrupt_state(true); // ensure idle thread interrupt enabled
    while (true) {
        // hlt: stop CPU until next interrupt (like clock)
        // so idle loop not 100% CPU
        asm volatile(
            "sti\n" // (Original)
            "hlt\n"
        );
        
        // (remove yield();)
        // clock_handler will handle hlt wakeup
        // check cfs_task_count and call schedule()
    }
}


static void user_init_thread() {
    u32 counter = 0;
    while (true) {
        sleep(1000);
    }
}

// (!!!!) NEW: counters for spinners (!!!!)
static volatile u64 counter_high = 0;
static volatile u64 counter_low = 0;

// (!!!!) NEW: high-weight thread (CPU-heavy) (!!!!)
void high_prio_spinner() {
    set_interrupt_state(true);
    // this thread never yields CPU
    while (true) {
        counter_high++;
    }
}

// (!!!!) NEW: low-weight thread (CPU-heavy) (!!!!)
void low_prio_spinner() {
    set_interrupt_state(true);
    // this thread also never yields CPU
    while (true) {
        counter_low++;
    }
}


void init_thread() {
    u64 last_high = 0;
    u64 last_low = 0; 
    // ...
    
    // loop print, observe counter growth rate
    while (true) {
        sleep(2000);
        
        u64 current_high = counter_high; 
        u64 current_low = counter_low;
        u64 delta_high = current_high - last_high;
        u64 delta_low = current_low - last_low;


        if (current_low > 0) {
            u64 total_ratio_scaled = (current_high * 100) / current_low; 
            
            printf("Total Ratio : %u.%02u\n", 
                    (u32)(total_ratio_scaled / 100), (u32)(total_ratio_scaled % 100));
        } else {
            printf("Total Ratio : N/A\n");
        }

        if (delta_low > 0) {
            u64 interval_ratio_scaled = (delta_high * 100) / delta_low;
            
            printf("Interval Ratio: %u.%02u\n", 
                    (u32)(interval_ratio_scaled / 100), (u32)(interval_ratio_scaled % 100));
        } else {
            printf("Interval Ratio: N/A\n");
        }

        last_high = current_high;
        last_low = current_low;
    }
}




void test_thread() {
    set_interrupt_state(true);
    u32 count = 0;
    while (true) {
        sleep(10000); // (Original was 2000)
    }
}