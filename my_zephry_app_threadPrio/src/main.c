#include <zephyr/kernel.h>        // Core kernel APIs: threads, semaphores, sleep, etc.
#include <zephyr/sys/printk.h>    // For printing to console
#include <zephyr/logging/log.h>   // Logging subsystem

// Register a log module for this application (optional, for structured logging)
LOG_MODULE_REGISTER(thread_states_demo);

// -------------------- Configuration --------------------
#define HIGH_PRIORITY 1           // Highest priority (lower number = higher priority)
#define STACK_SIZE 1024           // Stack size for each thread
#define PRIORITY 5                // Base priority for threads

// -------------------- Thread Resources --------------------
// Define stacks for each thread
K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread3_stack, STACK_SIZE);

// Define thread control blocks
struct k_thread thread1_data;
struct k_thread thread2_data;
struct k_thread thread3_data;

// Define a binary semaphore (initial count = 0, max count = 1)
// Used to simulate the "pending" state for Thread 2
K_SEM_DEFINE(sync_sem, 0, 1);

// -------------------- Thread Functions --------------------

/**
 * Thread 1: Demonstrates the "sleeping" and "suspended" states.
 * - Sleeps for 2 seconds in each loop iteration.
 * - Can be suspended and resumed by Thread 3.
 */
void thread1_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 1: Sleeping for 2 seconds (sleeping state)\n");
        k_sleep(K_SECONDS(2));  // Thread enters sleeping state
    }
}

/**
 * Thread 2: Demonstrates the "pending" state.
 * - Waits (blocks) on a semaphore.
 * - Gets unblocked when Thread 3 gives the semaphore.
 */
void thread2_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 2: Waiting on semaphore (pending state)\n");
        k_sem_take(&sync_sem, K_FOREVER);  // Thread enters pending state
        printk("Thread 2: Acquired semaphore!\n");
    }
}

/**
 * Thread 3: Demonstrates the "running" state and controls other threads.
 * - Suspends and resumes Thread 1.
 * - Gives the semaphore to unblock Thread 2.
 */
void thread3_fn(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 3: Running and suspending Thread 1\n");
        k_thread_suspend(&thread1_data);  // Thread 1 enters suspended state

        k_sleep(K_SECONDS(3));  // Simulate some processing time

        printk("Thread 3: Resuming Thread 1\n");
        k_thread_resume(&thread1_data);  // Thread 1 resumes execution

        printk("Thread 3: Giving semaphore to Thread 2\n");
        k_sem_give(&sync_sem);  // Unblocks Thread 2

        k_sleep(K_SECONDS(5));  // Wait before repeating the cycle
    }
}

// -------------------- Main Function --------------------

/**
 * Main function: Initializes and starts all threads.
 */
void main(void)
{
    printk("Starting thread state demo...\n");

    // Create Thread 1 with highest priority
    k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
                    thread1_fn, NULL, NULL, NULL,
                    HIGH_PRIORITY, 0, K_NO_WAIT);

    // Create Thread 2 with lowest priority
    k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
                    thread2_fn, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    // Create Thread 3 with medium priority
    k_thread_create(&thread3_data, thread3_stack, STACK_SIZE,
                    thread3_fn, NULL, NULL, NULL,
                    PRIORITY - 1, 0, K_NO_WAIT);
}


/*
```c
k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
                thread1_fn, NULL, NULL, NULL,
                HIGH_PRIORITY, 0, K_NO_WAIT);
```

### 🔍 What is `thread1_data`?

`thread1_data` is a **thread control block** of type `struct k_thread`. It is used by the Zephyr kernel to **track and manage the state and metadata** of the thread being created.

---

### 🧵 Role of `struct k_thread`

This structure holds internal information about the thread, such as:

- **Thread state** (e.g., ready, running, sleeping, suspended)
- **Priority**
- **Stack pointer**
- **Scheduling info**
- **Thread entry function and arguments**
- **Thread ID and name (if assigned)**

You declared it earlier in your code:

```c
struct k_thread thread1_data;
```

This is a **statically allocated thread descriptor**, which is passed to `k_thread_create()` so the kernel can initialize and manage the thread.

---

### 🧠 Why is it needed?

Unlike `K_THREAD_DEFINE()` which hides this detail, `k_thread_create()` requires you to explicitly provide:

- A **stack** (`thread1_stack`)
- A **control block** (`thread1_data`)
- A **function** to run (`thread1_fn`)
- **Priority and options**

This gives you **more control** over thread creation and lifecycle.

*/
