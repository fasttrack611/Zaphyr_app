#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/init.h>

#define STACK_SIZE 1024
#define THREAD_PRIORITY 5  // For timer Module 

#define MSGQ_SIZE 10
#define MSG_SIZE sizeof(int)

// Forward declarations for thread functions
void user_thread_fn(void *p1, void *p2, void *p3);
void user_thread_app_fn(void *p1, void *p2, void *p3);

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
    printk("  user_thread_fn: Running in user mode\n");

    if (k_msgq_get(&my_msgq, &received, K_FOREVER) == 0) {
        printk("  user_thread_fn: Received message: %d\n", received);
    } else {
        printk("  user_thread_fn: Failed to receive message\n");
    }
}

// Second user thread function
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

// PRE_KERNEL_1 init function
static int init_pre_kernel_1(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("   PRE_KERNEL_1: Early hardware setup\n");
    return 0;
}
SYS_INIT(init_pre_kernel_1, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// PRE_KERNEL_2 init function
static int init_pre_kernel_2(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("  PRE_KERNEL_2: Init device drivers (no kernel services)\n");
    return 0;
}
SYS_INIT(init_pre_kernel_2, PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// POST_KERNEL init function
static int start_user_thread(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("    POST_KERNEL: start_user_thread()\n");

    k_object_access_grant(&my_msgq, &user_thread_data);

    k_thread_create(&user_thread_data, user_stack_1, STACK_SIZE,
                    user_thread_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    int msg = 1042;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// APPLICATION init function
static int start_user_thread_app(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n  APPLICATION: start_user_thread_app()\n");

    k_object_access_grant(&my_msgq, &user_thread_APP);

    k_thread_create(&user_thread_APP, user_stack_2, STACK_SIZE,
                    user_thread_app_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    int msg = 1084;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread_app, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// SMP init function (only runs if SMP is enabled)
static int init_smp(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n SMP: Multi-core setup (Once enabled)\n");
    return 0;
}
SYS_INIT(init_smp, SMP, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);



void timing_thread(void)
{
    printk("Timing thread started\n");
}


K_THREAD_DEFINE(timing_tid, STACK_SIZE, timing_thread, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);
				
// Optional main() function
void main(void) {
    printk("\n IN MAIN --> main(): Application thread started....\n");
}

