#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define PRIORITY 5

// Declare delayable work items
struct k_work_delayable work1;
struct k_work_delayable work2;

// Shared work handler
void deferred_work_handler(struct k_work *work) {
    if (work == &work1.work) {
        printk(" Deferred work from Timer 1\n");
    } else if (work == &work2.work) {
        printk(" Deferred work from Timer 2\n");
    } else {
        printk(" Unknown work source\n");
    }
}

// ISR-like timer expiry functions
void timer1_expiry(struct k_timer *timer) {
    printk(" Timer 1 interrupt fired\n");
    k_work_schedule(&work1, K_MSEC(500));  // Delay before thread-level processing
}

void timer2_expiry(struct k_timer *timer) {
    printk(" Timer 2 interrupt fired\n");
    k_work_schedule(&work2, K_MSEC(1000)); // Slightly longer delay
}

// Define two independent timers with static handlers
K_TIMER_DEFINE(timer1, timer1_expiry, NULL);
K_TIMER_DEFINE(timer2, timer2_expiry, NULL);

int main(void) {
    printk(" Zephyr multi-interrupt + delayed work demo started\n");

    // Initialize delayable work items with shared handler
    k_work_init_delayable(&work1, deferred_work_handler);
    k_work_init_delayable(&work2, deferred_work_handler);

    // Start timers: first fires in 1s, then every 3s/4s respectively
    k_timer_start(&timer1, K_SECONDS(1), K_SECONDS(3));
    k_timer_start(&timer2, K_SECONDS(2), K_SECONDS(4));

    while (1) {
        // Main thread stays idle while timers and work system operate
        k_sleep(K_SECONDS(10));
    }

    return 0;
}
