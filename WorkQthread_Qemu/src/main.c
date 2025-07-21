#include <zephyr/kernel.h>        // Core kernel APIs
#include <zephyr/sys/printk.h>    // For console output (printk)

// 🧵 Stack size and thread priority definitions
#define STACK_SIZE 1024
#define PRIORITY   -4  // Cooperative thread (non-preemptive)

// 🔧 Work item handler function
// Called asynchronously by the workqueue thread when the work item is processed
void work_handler(struct k_work *work) {
    printk("🔧 Workqueue handler: processing deferred task\n");
}

// 📦 Statically define a work item and bind it to the handler
K_WORK_DEFINE(my_work, work_handler);

// 📦 Create a dedicated stack and structure for a custom workqueue
K_THREAD_STACK_DEFINE(workq_stack, STACK_SIZE);
struct k_work_q my_workq;

/*
 * 🧵 Logger thread
 * - Periodically prints system uptime
 * - Submits a deferred task (work item) to the custom workqueue
 * - Demonstrates non-blocking task submission from an application thread
 */
void logger_thread(void *a, void *b, void *c) {
    while (1) {
        // ⏱️ Print current system uptime
        printk("🟢 Logger thread: uptime = %lld ms\n", k_uptime_get());

        // 🔄 Submit the work item to the custom queue for deferred execution
        k_work_submit_to_queue(&my_workq, &my_work);

        // 💤 Sleep for 6 seconds before next dispatch
        k_sleep(K_SECONDS(6));
    }
}

// 🧵 Spawn the logger thread with cooperative priority
// Note: Cooperative threads must explicitly yield via sleep or API calls
K_THREAD_DEFINE(logger_id, STACK_SIZE, logger_thread, NULL, NULL, NULL,
                PRIORITY, 0, 0);

/*
 * 🧵 Main thread
 * - Initializes the custom workqueue with its own stack and thread context
 * - Remains idle in a sleep loop (can be used for diagnostics or board control)
 */
int main(void) {
    printk("🚀 Workqueue + logger thread demo started\n");

    // 🔧 Start the custom workqueue with given stack and thread priority
    k_work_queue_start(&my_workq, workq_stack,
                       K_THREAD_STACK_SIZEOF(workq_stack),
                       PRIORITY, NULL);

    // 💤 Let the main thread remain idle while other threads do the work
    while (1) {
        k_sleep(K_SECONDS(10));
    }
}
