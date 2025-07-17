#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY -4

// Work item handler
void work_handler(struct k_work *work) {
    printk("Workqueue handler: processing deferred task\n");
}

// Work item
K_WORK_DEFINE(my_work, work_handler);

// Custom workqueue
K_THREAD_STACK_DEFINE(workq_stack, STACK_SIZE);
struct k_work_q my_workq;

void logger_thread(void *a, void *b, void *c) {
    while (1) {
        printk("Logger thread: uptime = %lld ms\n", k_uptime_get());

        // Submit work item to custom workqueue
        k_work_submit_to_queue(&my_workq, &my_work);

        k_sleep(K_SECONDS(6));
    }
}

K_THREAD_DEFINE(logger_id, STACK_SIZE, logger_thread, NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void) {
    printk("Workqueue + additional thread demo started\n");

    // Start custom workqueue
    k_work_queue_start(&my_workq, workq_stack,
                       K_THREAD_STACK_SIZEOF(workq_stack),
                       PRIORITY, NULL);

    while (1) {
        k_sleep(K_SECONDS(10));  // Main thread stays idle
    }
}
