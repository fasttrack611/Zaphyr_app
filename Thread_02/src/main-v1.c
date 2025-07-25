#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>  // ✅ Needed for struct device
#include <zephyr/init.h>

#define STACK_SIZE 1024
#define MSGQ_SIZE 10
#define MSG_SIZE sizeof(int)

// Define separate stacks for each thread
K_THREAD_STACK_DEFINE(user_stack_1, STACK_SIZE);
K_THREAD_STACK_DEFINE(user_stack_2, STACK_SIZE);

// Thread control blocks
struct k_thread user_thread_data;
struct k_thread user_thread_APP;

// Message queue definition
K_MSGQ_DEFINE(my_msgq, MSG_SIZE, MSGQ_SIZE, 4);

// First user thread function
void user_thread_fn(void *p1, void *p2, void *p3)
{
    int received;
    printk("User thread: Running in user mode\n");

    if (k_msgq_get(&my_msgq, &received, K_FOREVER) == 0) {
        printk("User thread: Received message: %d\n", received);
    } else {
        printk("User thread: Failed to receive message\n");
    }
}

// Second user thread function (renamed to avoid conflict)
void user_thread_app_fn(void *p1, void *p2, void *p3)
{
    int received;
    printk("User thread APP: Running in user mode\n");

    if (k_msgq_get(&my_msgq, &received, K_FOREVER) == 0) {
        printk("User thread APP: Received message: %d\n", received);
    } else {
        printk("User thread APP: Failed to receive message\n");
    }
}

// SYS_INIT function to start the first thread
static int start_user_thread(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("Starting USER Thread..\n");

    k_object_access_grant(&my_msgq, &user_thread_data);

    k_thread_create(&user_thread_data, user_stack_1, STACK_SIZE,
                    user_thread_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    int msg = 1042;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}

// SYS_INIT function to start the second thread
static int start_user_thread_app(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("APP:: ( APPLICATION ) Starting USER Thread APP..\n");

    k_object_access_grant(&my_msgq, &user_thread_APP);

    k_thread_create(&user_thread_APP, user_stack_2, STACK_SIZE,
                    user_thread_app_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    int msg = 1084;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}

// Register both threads with SYS_INIT
SYS_INIT(start_user_thread, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
SYS_INIT(start_user_thread_app, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
