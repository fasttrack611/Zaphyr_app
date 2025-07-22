#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* Mutex with priority inheritance */
K_MUTEX_DEFINE(pi_mutex);

/* Thread control structures */
#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(low_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(med_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(high_stack, STACK_SIZE);

static struct k_thread low_thread, med_thread, high_thread;

/* Priority levels (lower number = higher priority) */
#define LOW_PRIO   5
#define MED_PRIO   4
#define HIGH_PRIO  3

volatile bool med_thread_ran = false;

void high_thread_entry(void *p1, void *p2, void *p3)
{
    printk("HIGH Thread STARTED (Prio: %d)\n", HIGH_PRIO);

    printk("HIGH Attempting to lock mutex...\n");
    k_mutex_lock(&pi_mutex, K_FOREVER);

    printk("HIGH Acquired mutex! Executing critical section...\n");
    k_busy_wait(USEC_PER_MSEC * 500); // 500ms busy work

    printk("HIGH Releasing mutex\n");
    k_mutex_unlock(&pi_mutex);

    printk("HIGH FINISHED\n");
}

void med_thread_entry(void *p1, void *p2, void *p3)
{
    printk("MED Thread STARTED (Prio: %d)\n", MED_PRIO);

    // Simulate work that would normally preempt low-prio thread
    med_thread_ran = true;
    printk("MED Thread executing (should NOT run during critical section!)\n");

    printk("MED FINISHED\n");
}

void low_thread_entry(void *p1, void *p2, void *p3)
{
    printk("LOW Thread STARTED (Prio: %d)\n", LOW_PRIO);

    printk("LOW Locking mutex...\n");
    k_mutex_lock(&pi_mutex, K_FOREVER);

    printk("LOW Mutex acquired. Current priority: %d\n",
           k_thread_priority_get(k_current_get()));

    // Start high-priority thread while holding mutex
    k_thread_create(&high_thread, high_stack, STACK_SIZE,
                   high_thread_entry, NULL, NULL, NULL,
                   HIGH_PRIO, 0, K_NO_WAIT);

    // Allow high thread to start and block on mutex
    k_sleep(K_MSEC(100));

    // Verify priority inheritance occurred
    int new_prio = k_thread_priority_get(k_current_get());
    printk("LOW Priority after inheritance: %d (expected %d)\n",
           new_prio, HIGH_PRIO);

    // Start medium-priority thread
    k_thread_create(&med_thread, med_stack, STACK_SIZE,
                   med_thread_entry, NULL, NULL, NULL,
                   MED_PRIO, 0, K_NO_WAIT);

    printk("LOW Doing work while holding mutex...\n");
    for (int i = 0; i < 3; i++) {
        printk("LOW Working %d/3 (should NOT be preempted by MED)\n", i+1);
        k_busy_wait(USEC_PER_MSEC * 300); // 300ms busy work
    }

    printk("LOW Releasing mutex\n");
    k_mutex_unlock(&pi_mutex);

    // Verify priority returned to original
    printk("LOW Priority after release: %d (expected %d)\n",
           k_thread_priority_get(k_current_get()), LOW_PRIO);

    printk("LOW FINISHED\n");
}

void main(void)
{
    printk("\nPriority Inheritance Demo on Zephyr (QEMU x86)\n");
    printk("==============================================\n");

    k_thread_create(&low_thread, low_stack, STACK_SIZE,
                   low_thread_entry, NULL, NULL, NULL,
                   LOW_PRIO, 0, K_NO_WAIT);

    k_sleep(K_SECONDS(5));

    printk("\nTest Summary:\n");
    printk("- Priority inheritance %s\n",
           (med_thread_ran ? "FAILED (MED ran during critical section)" : "SUCCESS"));
    printk("- Final priority restoration %s\n",
           (k_thread_priority_get(&low_thread) == LOW_PRIO ? "SUCCESS" : "FAILED"));

    printk("Demo complete. Halting QEMU.\n");
}
