#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread.h>  // Required for k_thread_suspend/resume

// ⚙️ Thread stack size and priority configuration
#define STACK_SIZE 1024
#define PRIORITY 5  // All threads use same priority for fairness in scheduling

// 🧵 Semaphore used for simulating "pending" thread state.
// Initial count = 0 ➤ thread trying to take it will block (go pending)
K_SEM_DEFINE(sync_sem, 0, 1);

/*
 * 💤 Sleeper Thread
 * - Goes into a timed sleep using k_sleep()
 * - Demonstrates the "sleeping" thread state
 */
void sleeper_thread(void *a, void *b, void *c) {
    while (1) {
        printk("\n🟡 Sleeper: sleeping for 2s\n");
        k_sleep(K_SECONDS(2));  // Thread enters sleeping state
        printk("\n🟡 Sleeper: awake and ready\n");
    }
}

/*
 * ⏳ Pending Thread
 * - Blocks while waiting for a semaphore
 * - Demonstrates the "pending" thread state (waiting for resource)
 */
void pending_thread(void *a, void *b, void *c) {
    while (1) {
        printk("🔵 Pending: waiting on semaphore\n");
        k_sem_take(&sync_sem, K_FOREVER);  // Thread blocks until semaphore is given
        printk("🔵 Pending: semaphore acquired\n");
    }
}

/*
 * 🧩 Controller Thread
 * - Controls state transitions of other threads
 * - Suspends/resumes sleeper thread
 * - Gives semaphore to pending thread
 */
void controller_thread(void *a, void *b, void *c) {
    k_tid_t sleeper_id = (k_tid_t)a;   // Sleeper thread ID
    k_tid_t pending_id = (k_tid_t)b;   // Pending thread ID

    while (1) {
        // 🧷 Suspend sleeper thread ➤ enters "suspended" state
        printk("🔴 Controller: suspending sleeper\n");
        k_thread_suspend(sleeper_id);

        k_sleep(K_SECONDS(3));  // Let it stay suspended for 3 seconds

        // ▶️ Resume sleeper ➤ re-enters "ready" state
        printk("🔴 Controller: resuming sleeper\n");
        k_thread_resume(sleeper_id);

        // 📤 Give semaphore ➤ pending thread becomes ready
        printk("🔴 Controller: giving semaphore to pending thread\n");
        k_sem_give(&sync_sem);

        k_sleep(K_SECONDS(4));  // Delay before next control cycle
    }
}
