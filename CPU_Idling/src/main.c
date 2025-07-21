#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 512
#define THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);

struct k_thread thread1_data;
struct k_thread thread2_data;

void thread1(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 1: Going to sleep for 1 second...\n");
        k_cpu_idle();
        printk("Thread 1: Woke up!\n");
        k_sleep(K_SECONDS(1));
    }
}

void thread2(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 2: Going to sleep for 2 seconds...\n");
        k_cpu_idle();
        printk("Thread 2: Woke up!\n");
        k_sleep(K_SECONDS(2));
    }
}

void main(void)
{
    printk("CPU Idling sample with 2 threads started\n");

    k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
                    thread1, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
                    thread2, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);
}
