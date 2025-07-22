#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5

K_MUTEX_DEFINE(mutex_a);
K_MUTEX_DEFINE(mutex_b);

void thread1(void *a, void *b, void *c) {
    while (1) {
        printk(" Thread 1: trying to lock mutex A\n");
        if (k_mutex_lock(&mutex_a, K_MSEC(500)) != 0) {
            printk("⚠️ Thread 1: failed to lock mutex A (timeout)\n");
            k_sleep(K_SECONDS(1));
            continue;
        }

        printk(" Thread 1: Aquared -> lock mutex A\n");
        k_sleep(K_MSEC(100));  // Simulate delay before next lock

        printk(" Thread 1: trying to lock mutex B\n");
        if (k_mutex_lock(&mutex_b, K_MSEC(500)) != 0) {
            printk("⚠️ Thread 1: failed to lock mutex B (timeout), releasing A\n");
            k_mutex_unlock(&mutex_a);
            k_sleep(K_SECONDS(1));
            continue;
        }

        printk(" Thread 1: acquired both mutexes\n");

        k_mutex_unlock(&mutex_b);
        k_mutex_unlock(&mutex_a);

        k_sleep(K_SECONDS(1));
    }
}

void thread2(void *a, void *b, void *c) {
    while (1) {
        printk(" Thread 2: trying to lock mutex B\n");
        if (k_mutex_lock(&mutex_b, K_MSEC(500)) != 0) {
            printk("⚠️ Thread 2: failed to lock mutex B (timeout)\n");
            k_sleep(K_SECONDS(1));
            continue;
        }

        printk(" Thread 2: Aquared -> lock mutex B\n");
        k_sleep(K_MSEC(100));  // Simulate delay before next lock

        printk(" Thread 2: trying to lock mutex A\n");
        if (k_mutex_lock(&mutex_a, K_MSEC(500)) != 0) {
            printk("⚠️ Thread 2: failed to lock mutex A (timeout), releasing B\n");
            k_mutex_unlock(&mutex_b);
            k_sleep(K_SECONDS(1));
            continue;
        }

        printk(" Thread 2: acquired both mutexes\n");

        k_mutex_unlock(&mutex_a);
        k_mutex_unlock(&mutex_b);

        k_sleep(K_SECONDS(1));
    }
}

K_THREAD_DEFINE(t1_id, STACK_SIZE, thread1, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(t2_id, STACK_SIZE, thread2, NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void) {
    printk(" Zephyr mutex timeout recovery demo started\n");
    return 0;
}
