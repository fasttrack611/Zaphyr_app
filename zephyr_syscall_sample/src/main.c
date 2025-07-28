#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/irq_offload.h>

K_THREAD_STACK_DEFINE(user_stack, 1024);
struct k_thread user_thread_data;

K_SEM_DEFINE(my_sem, 0, 1);

K_MSGQ_DEFINE(my_msgq, sizeof(int), 10, 4);

void user_thread(void *p1, void *p2, void *p3) {
    int msg = 42;
    printk("User thread: putting message %d\n", msg);
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);
    printk("User thread: giving semaphore\n");
    k_sem_give(&my_sem);
}

void main(void) {
    printk("Main thread: creating user thread\n");

    k_tid_t tid = k_thread_create(&user_thread_data, user_stack,
                                  K_THREAD_STACK_SIZEOF(user_stack),
                                  user_thread,
                                  NULL, NULL, NULL,
                                  1, K_USER, K_NO_WAIT);

    k_thread_name_set(tid, "user_thread");

    printk("Main thread: waiting for semaphore\n");
    k_sem_take(&my_sem, K_FOREVER);
    printk("Main thread: semaphore taken\n");

    int received;
    k_msgq_get(&my_msgq, &received, K_FOREVER);
    printk("Main thread: received message %d\n", received);
}
