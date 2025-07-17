#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

K_SEM_DEFINE(my_sem, 0, 1);  // ISR-triggered semaphore

// ISR-like timer handler
void timer_expiry_handler(struct k_timer *timer_id) {
    printk("ISR: Semaphore triggered\n");
    k_sem_give(&my_sem);
}

// Timer that simulates interrupt
K_TIMER_DEFINE(my_timer, timer_expiry_handler, NULL);

// Additional system thread
void logger_thread(void *a, void *b, void *c) {
    while (1) {
        printk("Logger thread: uptime = %lld ms\n", k_uptime_get());
        k_sleep(K_SECONDS(2));
    }
}

//K_THREAD_DEFINE(logger_id, 1024, logger_thread, NULL, NULL, NULL, 4, 0, 0);
K_THREAD_DEFINE(logger_id, 1024, logger_thread, NULL, NULL, NULL, -2, 0, 0);

int main(void) {
    printk("System thread + CPU idle demo started\n");

    k_timer_start(&my_timer, K_SECONDS(2), K_SECONDS(3));

    while (1) {
        if (k_sem_take(&my_sem, K_NO_WAIT) == 0) {
            printk("Main thread: got semaphore, doing work\n");
        } else {
            printk("Main thread: no work, idling CPU\n");
            k_cpu_idle();
	    k_yield();  // Give other threads a chance to run
        }
    }
}
