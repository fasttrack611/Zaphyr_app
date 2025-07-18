#include <zephyr/kernel.h>       // Core kernel APIs (threads, semaphores, sleep, etc.)
#include <zephyr/sys/printk.h>   // For printing to console
#include <zephyr/logging/log.h>  // Logging subsystem

// Register a log module for this application
LOG_MODULE_REGISTER(thread_states_demo);

// Define priorities and stack size
#define HIGH_PRIORITY 1          // Higher priority (lower number = higher priority)
#define STACK_SIZE 1024          // Stack size for each thread
#define PRIORITY 5               // Base priority for threads

// Define stacks for each thread
K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread3_stack, STACK_SIZE);

// Define thread control blocks
struct k_thread thread1_data;
struct k_thread thread2_data;
struct k_thread thread3_data;

// Define a semaphore with initial count 0 and max count 1
K_SEM_DEFINE(sync_sem, 0, 1);  // Used to simulate pending state

// Thread 1: Demonstrates sleeping and suspended states
void thread1_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 1: Sleeping for 2 seconds (sleeping state)\n");
        k_sleep(K_SECONDS(2));  // Thread goes to sleep
    }
}

// Thread 2: Demonstrates pending state (waiting on semaphore)
void thread2_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 2: Waiting on semaphore (pending state)\n");
        k_sem_take(&sync_sem, K_FOREVER);  // Blocks until semaphore is given
        printk("Thread 2: Acquired semaphore!\n");
    }
}

// Thread 3: Demonstrates running state and controls other threads
void thread3_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 3: Running and suspending Thread 1\n");
        k_thread_suspend(&thread1_data);  // Suspend Thread 1

        k_sleep(K_SECONDS(3));  // Simulate some processing time

        printk("Thread 3: Resuming Thread 1\n");
        k_thread_resume(&thread1_data);  // Resume Thread 1

        printk("Thread 3: Giving semaphore to Thread 2\n");
        k_sem_give(&sync_sem);  // Unblock Thread 2

        k_sleep(K_SECONDS(5));  // Wait before repeating
    }
}

// Main function: creates and starts the threads
void main(void)
{
    printk("Starting thread state demo...\n");

    // Create Thread 1 with high priority
    k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
                    thread1_fn, NULL, NULL, NULL,
                    HIGH_PRIORITY, 0, K_NO_WAIT);

    // Create Thread 2 with lower priority
    k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
                    thread2_fn, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    // Create Thread 3 with medium priority (between Thread 1 and 2)
    k_thread_create(&thread3_data, thread3_stack, STACK_SIZE,
                    thread3_fn, NULL, NULL, NULL,
                    PRIORITY - 1, 0, K_NO_WAIT);
}




/*
Summary of Thread Behavior
Thread	Priority	Behavior
Thread 1	1 (High)	Sleeps for 2s, can be suspended/resumed by Thread 3
Thread 2	5	Waits (pends) on a semaphore, unblocked by Thread 3
Thread 3	4	Controls Thread 1 and 2, demonstrates running state

*/
