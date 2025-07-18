#include <zephyr/kernel.h>       // Core kernel APIs: threads, sleep, etc.
#include <zephyr/sys/printk.h>   // For printing messages to the console
#include <zephyr/sys/util.h>

/* Define a struct for messages */
struct message {
    uint32_t counter;
};

/* Create a message queue */
K_MSGQ_DEFINE(my_msgq, sizeof(struct message), 10, 4);

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
void producer_thread(void *arg1, void *arg2, void *arg3)
{
    struct message msg = {0};
    while (1) {
        msg.counter++;
        if (k_msgq_put(&my_msgq, &msg, K_NO_WAIT) != 0) {
            k_msgq_purge(&my_msgq);
        }
        printk("Producer sent: %d\n", msg.counter);
        k_sleep(K_MSEC(500));
    }
}

void consumer_thread(void *arg1, void *arg2, void *arg3)
{
    struct message msg;
    while (1) {
        if (k_msgq_get(&my_msgq, &msg, K_FOREVER) == 0) {
            printk("Consumer received: %d\n", msg.counter);
        }
    }
}

// ---------------------------------------------
// Main function: entry point of the application
// Creates and starts 4 threads with different priorities and arguments
// ---------------------------------------------
void main(void)
{
    printk("Starting producer-consumer demo\n");

    // Create producer thread
    k_thread_create(&thread1_data, stack1, STACK_SIZE,
                    producer_thread, NULL, NULL, NULL,
                    PRIORITY_1, 0, K_NO_WAIT);

    // Create consumer thread
    k_thread_create(&thread2_data, stack2, STACK_SIZE,
                    consumer_thread, NULL, NULL, NULL,
                    PRIORITY_2, 0, K_NO_WAIT);
}
