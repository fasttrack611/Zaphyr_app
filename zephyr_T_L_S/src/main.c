#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define STACK_SIZE 1024
#define PRIORITY 5

// Declare a thread-local variable
Z_THREAD_LOCAL int tls_counter = 0;

K_THREAD_STACK_DEFINE(stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, STACK_SIZE);
struct k_thread thread1_data;
struct k_thread thread2_data;

void thread_func(void *p1, void *p2, void *p3)
{
    const char *name = (const char *)p1;

    for (int i = 0; i < 5; i++) {
        tls_counter++;
        printf("%s: tls_counter = %d\n", name, tls_counter);
        k_sleep(K_MSEC(500));
    }
}

void main(void)
{
    printk("Main thread started\n");

    k_thread_create(&thread1_data, stack1, STACK_SIZE,
                    thread_func, "Thread A", NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread2_data, stack2, STACK_SIZE,
                    thread_func, "Thread B", NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);
}

