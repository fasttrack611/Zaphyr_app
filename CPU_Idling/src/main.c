#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// ⚙️ Configuration Macros
#define STACK_SIZE      512        // Stack size for each thread
#define THREAD_PRIORITY 5          // Thread priority (higher number = lower priority)

// 🧵 Thread stack declarations
K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);

// Thread control block structures
struct k_thread thread1_data;
struct k_thread thread2_data;

/*
 * thread1: Simulates short sleep with CPU idle
 * - Calls k_cpu_idle() to signal the kernel that the CPU can be put into idle state.
 * - Then sleeps explicitly using k_sleep().
 */
void thread1(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk(" Thread 1: Requesting CPU idle\n");
        k_cpu_idle();  // Hint to kernel to put CPU into low-power idle until interrupt
        printk(" Thread 1: Back from idle\n");
        k_sleep(K_SECONDS(1));  // Normal sleep — this thread yields to scheduler
    }
}

/*
 * 🧵 thread2: Simulates longer sleep with CPU idle
 * - Same pattern as thread1, but sleeps for a longer duration
 */
void thread2(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk(" Thread 2: Requesting CPU idle\n");
        k_cpu_idle();  // CPU goes idle if no interrupts or tasks
        printk(" Thread 2: Back from idle\n");
        k_sleep(K_SECONDS(2));  // Sleep for 2 seconds
    }
}

/*
 * 🧵 main(): Starts both threads
 * - No user logic here, only thread creation
 */
void main(void)
{
    printk(" Main: CPU idling sample started with 2 threads\n");

    // Create thread 1
    k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
                    thread1, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    // Create thread 2
    k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
                    thread2, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);
}

