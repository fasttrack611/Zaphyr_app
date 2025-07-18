#include <zephyr/kernel.h>       // Core kernel APIs: threads, sleep, etc.
#include <zephyr/sys/printk.h>   // For printing messages to the console

// Define the stack size for each thread
#define STACK_SIZE 1024

// Define thread priorities (lower number = higher priority)
#define PRIORITY_1 1  // Highest priority
#define PRIORITY_2 2
#define PRIORITY_3 3
#define PRIORITY_4 4  // Lowest priority

// Allocate memory for thread stacks
K_THREAD_STACK_DEFINE(stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack3, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack4, STACK_SIZE);

// Declare thread control blocks
struct k_thread thread1_data;
struct k_thread thread2_data;
struct k_thread thread3_data;
struct k_thread thread4_data;

// ---------------------------------------------
// Thread function: accepts 3 arguments
// arg1: thread name (string)
// arg2: delay in milliseconds (casted to void*)
// arg3: unused (can be NULL)
// ---------------------------------------------
void thread_fn(void *arg1, void *arg2, void *arg3)
{
    const char *name = (const char *)arg1;               // Convert arg1 to string
    int delay = (int)(uintptr_t)arg2;                    // Convert arg2 to int

    while (1) {
        printk("%s: Running with delay %d ms\n", name, delay);
        k_sleep(K_MSEC(delay));                          // Sleep for specified delay
    }
}

// ---------------------------------------------
// Main function: entry point of the application
// Creates and starts 4 threads with different priorities and arguments
// ---------------------------------------------
void main(void)
{
    printk("Starting 4-thread demo with different priorities and arguments\n");

    // Create Thread A with highest priority and 500ms delay
    k_thread_create(&thread1_data, stack1, STACK_SIZE,
                    thread_fn, "Thread A", (void *)500, NULL,
                    PRIORITY_1, 0, K_NO_WAIT);

    // Create Thread B with priority 2 and 1000ms delay
    k_thread_create(&thread2_data, stack2, STACK_SIZE,
                    thread_fn, "Thread B", (void *)1000, NULL,
                    PRIORITY_2, 0, K_NO_WAIT);

    // Create Thread C with priority 3 and 1500ms delay
    k_thread_create(&thread3_data, stack3, STACK_SIZE,
                    thread_fn, "Thread C", (void *)1500, NULL,
                    PRIORITY_3, 0, K_NO_WAIT);

    // Create Thread D with lowest priority and 2000ms delay
    k_thread_create(&thread4_data, stack4, STACK_SIZE,
                    thread_fn, "Thread D", (void *)2000, NULL,
                    PRIORITY_4, 0, K_NO_WAIT);
}
