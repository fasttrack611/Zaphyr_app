#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

// Define thread priorities (lower number = higher priority)
#define PRIORITY_1 1
#define PRIORITY_2 2
#define PRIORITY_3 3
#define PRIORITY_4 4

// Thread stack definitions
K_THREAD_STACK_DEFINE(stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack3, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack4, STACK_SIZE);

// Thread control blocks
struct k_thread thread1_data;
struct k_thread thread2_data;
struct k_thread thread3_data;
struct k_thread thread4_data;

// Thread function that accepts arguments
void thread_fn(void *arg1, void *arg2, void *arg3)
{
    const char *name = (const char *)arg1;
    int delay = (int)(uintptr_t)arg2;

    while (1) {
        printk("%s: Running with delay %d ms\n", name, delay);
        k_sleep(K_MSEC(delay));
    }
}

void main(void)
{
    printk("Starting 4-thread demo with different priorities and arguments\n");

    // Create 4 threads with different priorities and arguments
    k_thread_create(&thread1_data, stack1, STACK_SIZE,
                    thread_fn, "Thread A", (void *)500, NULL,
                    PRIORITY_1, 0, K_NO_WAIT);

    k_thread_create(&thread2_data, stack2, STACK_SIZE,
                    thread_fn, "Thread B", (void *)1000, NULL,
                    PRIORITY_2, 0, K_NO_WAIT);

    k_thread_create(&thread3_data, stack3, STACK_SIZE,
                    thread_fn, "Thread C", (void *)1500, NULL,
                    PRIORITY_3, 0, K_NO_WAIT);

    k_thread_create(&thread4_data, stack4, STACK_SIZE,
                    thread_fn, "Thread D", (void *)2000, NULL,
                    PRIORITY_4, 0, K_NO_WAIT);
}

