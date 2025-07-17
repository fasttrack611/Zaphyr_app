#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(thread_states_demo);

#define HIGH_PRIORITY 1

#define STACK_SIZE 1024
#define PRIORITY 5

K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread3_stack, STACK_SIZE);

struct k_thread thread1_data;
struct k_thread thread2_data;
struct k_thread thread3_data;

K_SEM_DEFINE(sync_sem, 0, 1);  // Used to simulate pending state

void thread1_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 1: Sleeping for 2 seconds (sleeping state)\n");
        k_sleep(K_SECONDS(2));
    }
}

void thread2_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 2: Waiting on semaphore (pending state)\n");
        k_sem_take(&sync_sem, K_FOREVER);
        printk("Thread 2: Acquired semaphore!\n");
    }
}

void thread3_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 3: Running and suspending Thread 1\n");
        k_thread_suspend(&thread1_data);
        k_sleep(K_SECONDS(3));
        printk("Thread 3: Resuming Thread 1\n");
        k_thread_resume(&thread1_data);

        printk("Thread 3: Giving semaphore to Thread 2\n");
        k_sem_give(&sync_sem);

        k_sleep(K_SECONDS(5));
    }
}

void main(void)
{
    printk("Starting thread state demo...\n");

    k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
                    thread1_fn, NULL, NULL, NULL,
                    HIGH_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
                    thread2_fn, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread3_data, thread3_stack, STACK_SIZE,
                    thread3_fn, NULL, NULL, NULL,
                    PRIORITY - 1, 0, K_NO_WAIT);  // Higher priority
}

