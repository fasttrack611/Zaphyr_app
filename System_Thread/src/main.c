#include <zephyr/kernel.h>         // Core kernel APIs
#include <zephyr/sys/printk.h>     // printk() for console output
#include <zephyr/kernel/thread.h>  // Thread management APIs

/*k_thread_name_set() ensures the thread names appear in the listing.
k_thread_foreach() iterates over all active threads, including system ones like main and idle.
You can adjust PRIORITY1 and PRIORITY2 to experiment with scheduling behavior.

*/

// Define stack size and priorities for the two threads
#define STACK_SIZE 512
#define PRIORITY1 1
#define PRIORITY2 2

// Allocate stack memory and thread control blocks
K_THREAD_STACK_DEFINE(stack_area1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area2, STACK_SIZE);
struct k_thread thread_data1;
struct k_thread thread_data2;

// Thread function that prints a message 5 times with 1-second delay
void thread_func(void *p1, void *p2, void *p3) {
    int count = 5;
    while (count--) {
        printk("Thread %s running\n", (char *)p1);
        k_sleep(K_MSEC(1000));
    }
}

// Callback function to print information about each thread
void print_thread_info(const struct k_thread *thread, void *user_data) {
    const char *name = k_thread_name_get((k_tid_t)thread);  // Get thread name
    int prio = thread->base.prio;                           // Get thread priority
    size_t stack_size = thread->stack_info.size;            // Get stack size

    printk("Thread: %-12s | Priority: %2d | Stack size: %4zu bytes\n",
           name ? name : "Unnamed",
           prio,
           stack_size);
}

// Main function: creates threads and lists all threads
void main(void) {
    printk("\n Listing all threads (including system threads):\n\n");

    // Create thread T1 with priority 1
    k_thread_create(&thread_data1, stack_area1, STACK_SIZE,
                    thread_func, "T1", NULL, NULL,
                    PRIORITY1, 0, K_NO_WAIT);
    k_thread_name_set(&thread_data1, "T1");  // Set thread name for visibility

    // Create thread T2 with priority 2
    k_thread_create(&thread_data2, stack_area2, STACK_SIZE,
                    thread_func, "T2", NULL, NULL,
                    PRIORITY2, 0, K_NO_WAIT);
    k_thread_name_set(&thread_data2, "T2");  // Set thread name for visibility

    // List all threads using the callback
    k_thread_foreach(print_thread_info, NULL);
}
