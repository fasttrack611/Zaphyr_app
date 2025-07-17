#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread.h>

#define STACK_SIZE 1024
#define PRIORITY 5

K_SEM_DEFINE(sync_sem, 0, 1);  // Used to simulate pending state

void sleeper_thread(void *a, void *b, void *c) {
    while (1) {
        printk("\n Sleeper: sleeping for 2s\n");
        k_sleep(K_SECONDS(2));  // Thread enters sleeping state
        printk("\n Sleeper: awake and ready\n");
    }
}

void pending_thread(void *a, void *b, void *c) {
    while (1) {
        printk(" Pending: waiting on semaphore\n");
        k_sem_take(&sync_sem, K_FOREVER);  // Thread enters pending state
        printk(" Pending: semaphore acquired\n");
    }
}

void controller_thread(void *a, void *b, void *c) {
    k_tid_t sleeper_id = (k_tid_t)a;
    k_tid_t pending_id = (k_tid_t)b;

    while (1) {
        printk(" Controller: suspending sleeper\n");
        k_thread_suspend(sleeper_id);  // Sleeper enters suspended state

        k_sleep(K_SECONDS(3));

        printk(" Controller: resuming sleeper\n");
        k_thread_resume(sleeper_id);  // Sleeper returns to ready state

        printk(" Controller: giving semaphore to pending thread\n");
        k_sem_give(&sync_sem);  // Pending thread becomes ready

        k_sleep(K_SECONDS(4));
    }
}

K_THREAD_STACK_DEFINE(sleeper_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pending_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(controller_stack, STACK_SIZE);

struct k_thread sleeper_data;
struct k_thread pending_data;
struct k_thread controller_data;

int main(void) {
    printk(" Zephyr Thread State Demo Started\n");

    k_tid_t sleeper_id = k_thread_create(&sleeper_data, sleeper_stack,
        STACK_SIZE, sleeper_thread, NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);

    k_tid_t pending_id = k_thread_create(&pending_data, pending_stack,
        STACK_SIZE, pending_thread, NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&controller_data, controller_stack,
        STACK_SIZE, controller_thread, (void *)sleeper_id, (void *)pending_id, NULL,
        PRIORITY, 0, K_NO_WAIT);

    return 0;
}
