#include <zephyr/kernel.h>       // Core kernel APIs (threads, scheduling, etc.)
#include <zephyr/sys/printk.h>   // printk() for console output
#include <zephyr/device.h>       // Device structure used in SYS_INIT
#include <zephyr/init.h>         // SYS_INIT macros for boot-time initialization

// -----------------------------
// Configuration Constants
// -----------------------------

#define STACK_SIZE 1024          // Stack size for each thread
#define THREAD_PRIORITY 5        // Priority for the timing thread

#define MSGQ_SIZE 10             // Number of messages the queue can hold
#define MSG_SIZE sizeof(int)     // Size of each message (integer)

// -----------------------------
// Forward Declarations
// -----------------------------

void user_thread_fn(void *p1, void *p2, void *p3);       // First user thread
void user_thread_app_fn(void *p1, void *p2, void *p3);   // Second user thread

// -----------------------------
// Kernel Objects
// -----------------------------

// Define stacks for each thread
K_THREAD_STACK_DEFINE(user_stack_1, STACK_SIZE);
K_THREAD_STACK_DEFINE(user_stack_2, STACK_SIZE);

// Thread control blocks (TCBs)
struct k_thread user_thread_data;
struct k_thread user_thread_APP;

// Define a message queue to send integers between threads
// Arguments: name, message size, queue length, alignment
K_MSGQ_DEFINE(my_msgq, MSG_SIZE, MSGQ_SIZE, 4);

// -----------------------------
// Thread Functions
// -----------------------------

// First user thread: waits for a message and prints it
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

// Second user thread: same as above, but different message
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

// -----------------------------
// SYS_INIT Functions
// -----------------------------

// PRE_KERNEL_1: runs before kernel services are available
static int init_pre_kernel_1(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("   PRE_KERNEL_1: Early hardware setup\n");
    return 0;
}
SYS_INIT(init_pre_kernel_1, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// PRE_KERNEL_2: runs after PRE_KERNEL_1, still no kernel services
static int init_pre_kernel_2(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("  PRE_KERNEL_2: Init device drivers (no kernel services)\n");
    return 0;
}
SYS_INIT(init_pre_kernel_2, PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// POST_KERNEL: kernel services are available, safe to create threads
static int start_user_thread(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("    POST_KERNEL: start_user_thread()\n");

    // Grant access to the message queue for this thread
    k_object_access_grant(&my_msgq, &user_thread_data);

    // Create the first user thread
    k_thread_create(&user_thread_data, user_stack_1, STACK_SIZE,
                    user_thread_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);  // Priority 1, user mode, no delay

    // Send a message to the queue
    int msg = 1042;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// APPLICATION: runs after all kernel and driver init is done
static int start_user_thread_app(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n  APPLICATION: start_user_thread_app()\n");

    // Grant access to the message queue for this thread
    k_object_access_grant(&my_msgq, &user_thread_APP);

    // Create the second user thread
    k_thread_create(&user_thread_APP, user_stack_2, STACK_SIZE,
                    user_thread_app_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);  // Priority 1, user mode, no delay

    // Send a message to the queue
    int msg = 1084;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread_app, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// SMP: runs only if Symmetric Multi-Processing is enabled
static int init_smp(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n SMP: Multi-core setup (Once enabled)\n");
    return 0;
}
SYS_INIT(init_smp, SMP, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// -----------------------------
// Timing Thread (Optional)
// -----------------------------

// Simple thread that prints a message
void timing_thread(void)
{
    printk("Timing thread started\n");
}

// Define the timing thread at build time
// Arguments: name, stack size, entry function, args, priority, options, delay
K_THREAD_DEFINE(timing_tid, STACK_SIZE, timing_thread, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);

// -----------------------------
// Main Function (Optional)
// -----------------------------
void main(void)
{
    printk("\n IN MAIN --> main(): Application thread started....\n");
}


/* Here’s a **fully commented and explained version** of your Zephyr RTOS application, with detailed descriptions for each macro, function, and argument. This will help you understand **how the system initializes**, **how threads are created**, and **how inter-thread communication via message queues works**.

---

### ✅ Full Code with Detailed Comments

```c
#include <zephyr/kernel.h>       // Core kernel APIs (threads, scheduling, etc.)
#include <zephyr/sys/printk.h>   // printk() for console output
#include <zephyr/device.h>       // Device structure used in SYS_INIT
#include <zephyr/init.h>         // SYS_INIT macros for boot-time initialization

// -----------------------------
// Configuration Constants
// -----------------------------

#define STACK_SIZE 1024          // Stack size for each thread
#define THREAD_PRIORITY 5        // Priority for the timing thread

#define MSGQ_SIZE 10             // Number of messages the queue can hold
#define MSG_SIZE sizeof(int)     // Size of each message (integer)

// -----------------------------
// Forward Declarations
// -----------------------------

void user_thread_fn(void *p1, void *p2, void *p3);       // First user thread
void user_thread_app_fn(void *p1, void *p2, void *p3);   // Second user thread

// -----------------------------
// Kernel Objects
// -----------------------------

// Define stacks for each thread
K_THREAD_STACK_DEFINE(user_stack_1, STACK_SIZE);
K_THREAD_STACK_DEFINE(user_stack_2, STACK_SIZE);

// Thread control blocks (TCBs)
struct k_thread user_thread_data;
struct k_thread user_thread_APP;

// Define a message queue to send integers between threads
// Arguments: name, message size, queue length, alignment
K_MSGQ_DEFINE(my_msgq, MSG_SIZE, MSGQ_SIZE, 4);

// -----------------------------
// Thread Functions
// -----------------------------

// First user thread: waits for a message and prints it
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

// Second user thread: same as above, but different message
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

// -----------------------------
// SYS_INIT Functions
// -----------------------------

// PRE_KERNEL_1: runs before kernel services are available
static int init_pre_kernel_1(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("   PRE_KERNEL_1: Early hardware setup\n");
    return 0;
}
SYS_INIT(init_pre_kernel_1, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// PRE_KERNEL_2: runs after PRE_KERNEL_1, still no kernel services
static int init_pre_kernel_2(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("  PRE_KERNEL_2: Init device drivers (no kernel services)\n");
    return 0;
}
SYS_INIT(init_pre_kernel_2, PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// POST_KERNEL: kernel services are available, safe to create threads
static int start_user_thread(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("    POST_KERNEL: start_user_thread()\n");

    // Grant access to the message queue for this thread
    k_object_access_grant(&my_msgq, &user_thread_data);

    // Create the first user thread
    k_thread_create(&user_thread_data, user_stack_1, STACK_SIZE,
                    user_thread_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);  // Priority 1, user mode, no delay

    // Send a message to the queue
    int msg = 1042;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// APPLICATION: runs after all kernel and driver init is done
static int start_user_thread_app(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n  APPLICATION: start_user_thread_app()\n");

    // Grant access to the message queue for this thread
    k_object_access_grant(&my_msgq, &user_thread_APP);

    // Create the second user thread
    k_thread_create(&user_thread_APP, user_stack_2, STACK_SIZE,
                    user_thread_app_fn, NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);  // Priority 1, user mode, no delay

    // Send a message to the queue
    int msg = 1084;
    k_msgq_put(&my_msgq, &msg, K_NO_WAIT);

    return 0;
}
SYS_INIT(start_user_thread_app, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// SMP: runs only if Symmetric Multi-Processing is enabled
static int init_smp(const struct device *dev)
{
    ARG_UNUSED(dev);
    printk("\n SMP: Multi-core setup (Once enabled)\n");
    return 0;
}
SYS_INIT(init_smp, SMP, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

// -----------------------------
// Timing Thread (Optional)
// -----------------------------

// Simple thread that prints a message
void timing_thread(void)
{
    printk("Timing thread started\n");
}

// Define the timing thread at build time
// Arguments: name, stack size, entry function, args, priority, options, delay
K_THREAD_DEFINE(timing_tid, STACK_SIZE, timing_thread, NULL, NULL, NULL,
                THREAD_PRIORITY, 0, 0);

// -----------------------------
// Main Function (Optional)
// -----------------------------

void main(void)
{
    printk("\n IN MAIN --> main(): Application thread started....\n");
}
```

---

### 🧭 Execution Flow Summary

1. **PRE_KERNEL_1** → `init_pre_kernel_1()`
2. **PRE_KERNEL_2** → `init_pre_kernel_2()`
3. **POST_KERNEL** → `start_user_thread()` → creates `user_thread_fn`
4. **APPLICATION** → `start_user_thread_app()` → creates `user_thread_app_fn`
5. **SMP** → `init_smp()` (only if SMP is enabled)
6. **main()** → runs in its own thread
7. **timing_thread** → runs in parallel

---

Would you like a **visual boot flow diagram** or a **QEMU-ready build script** to test this?
*/
