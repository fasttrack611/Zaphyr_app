#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024

#define STACK_SIZE 1024
#define MSGQ_SIZE 10
#define MSG_SIZE sizeof(int)

K_THREAD_STACK_DEFINE(user_stack, STACK_SIZE);
struct k_thread user_thread_data;

K_MSGQ_DEFINE(my_msgq, MSG_SIZE, MSGQ_SIZE, 4);

void user_thread(void *p1, void *p2, void *p3)
{
    int received;

    printk("User thread: Running in user mode\n");

    if (k_msgq_get(&my_msgq, &received, K_FOREVER) == 0) {
        printk("User thread: Received message: %d\n", received);
    } else {
        printk("User thread: Failed to receive message\n");
    }
}

void main(void)
{
    printk("Main thread: Running in kernel mode\n");

    // Grant access to message queue
    k_object_access_grant(&my_msgq, &user_thread_data);

    // Create user thread
    k_thread_create(&user_thread_data, user_stack, STACK_SIZE,
                    user_thread, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    // Send message to user thread
    int msg = 42;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);
}

