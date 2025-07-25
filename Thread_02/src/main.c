#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/init.h>

// Stack size and thread priority definitions
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5  // Priority for the timing thread

// Message queue configuration
#define MSGQ_SIZE 10
#define MSG_SIZE sizeof(int)

// Forward declarations for user threads
void user_thread_fn(void *p1, void *p2, void *p3);
void user_thread_app_fn(void *p1, void *p2, void *p3);

// Define stacks for each thread
K_THREAD_STACK_DEFINE(user_stack_1, STACK_SIZE);
K_THREAD_STACK_DEFINE(user_stack_2, STACK_SIZE);

// Thread control blocks
struct k_thread user_thread_data;
struct k_thread user_thread_APP;

// Define a message queue with 10 messages of type int
K_MSGQ_DEFINE(my_msgq, MSG_SIZE, MSGQ_SIZE, 4);

// First user thread: waits for and prints a message from the queue
void user_thread_fn(void *p1, void *p2, void *p3)
{
    int received;
    printk("  user_thread_fn: Running in user mode\n");

    if (k_msgq_get(&my_msgq, &received, K_FOREVER) == 0) {
        printk("  user_thread_fn: Received message: %d\n", received);
    } else {
        printk("  user_thread_fn: Failed to receive message\n");
    }
}

// Second user thread: also waits for and prints a message from the queue
void user_thread_app_fn(void *p1, void *p2, void *p3)
{
    int received;
    printk("  user_thread_app_fn: Running in user mode\n");

    if (k_msgq_get(&my_msgq, &received, K_FOREVER) == 0) {
        printk("  user_thread_app_fn: Received message: %d\n", received);
    } else {
        printk("  user_thread_app_fn: Failed to receive message\n");
    }
}

// PRE_KERNEL_1 init: runs before kernel services are available
static int init_pre_kernel_1(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("   PRE_KERNEL_1: Early hardware setup\n");
    return 0;
}
SYS_INIT(init_pre_kernel_1, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// PRE_KERNEL_2 init: runs after PRE_KERNEL_1, still no kernel services
static int init_pre_kernel_2(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("  PRE_KERNEL_2: Init device drivers (no kernel services)\n");
    return 0;
}
SYS_INIT(init_pre_kernel_2, PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// POST_KERNEL init: kernel services are available, start first user thread
static int start_user_thread(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("    POST_KERNEL: start_user_thread()\n");

    // Grant access to the message queue for the thread
    k_object_access_grant(&my_msgq, &user_thread_data);

    // Create the first user thread
    k_thread_create(&user_thread_data, user_stack_1, STACK_SIZE,
                    user_thread_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    // Send a message to the queue
    int msg = 1042;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// APPLICATION init: start second user thread
static int start_user_thread_app(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n  APPLICATION: start_user_thread_app()\n");

    // Grant access to the message queue for the second thread
    k_object_access_grant(&my_msgq, &user_thread_APP);

    // Create the second user thread
    k_thread_create(&user_thread_APP, user_stack_2, STACK_SIZE,
                    user_thread_app_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    // Send a message to the queue
    int msg = 1084;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread_app, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// SMP init: runs only if SMP (Symmetric Multi-Processing) is enabled
static int init_smp(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n SMP: Multi-core setup (Once enabled)\n");
    return 0;
}
SYS_INIT(init_smp, SMP, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// Timing thread function
void timing_thread(void)
{
    printk("Timing thread started\n");
}

// Define the timing thread at build time
K_THREAD_DEFINE(timing_tid, STACK_SIZE, timing_thread, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);

// Optional main() function
void main(void) {
    printk("\n IN MAIN --> main(): Application thread started....\n");
}
