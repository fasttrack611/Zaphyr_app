#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// Thread stack size and priorities
#define STACK_SIZE 1024
#define THREAD_A_PRIO 5
#define THREAD_B_PRIO 5

// Declare synchronization primitives
K_MUTEX_DEFINE(mutex);              // Mutex to protect access to shared flag
K_CONDVAR_DEFINE(condvar);          // Condition variable for signaling between threads

volatile bool flag_ready = false;   // Shared flag indicating readiness

// Thread A: Waits for the condition to be met
void thread_a_fn(void)
{
    // Lock the mutex before checking the condition
    k_mutex_lock(&mutex, K_FOREVER);

    // Wait until flag_ready becomes true
    while (!flag_ready) {
        printk("Thread A: waiting for condition...\n");

        // Wait on the condition variable
        // This releases the mutex and puts the thread to sleep
        // When signaled, it re-acquires the mutex before continuing
        k_condvar_wait(&condvar, &mutex, K_FOREVER);
    }

    printk("Thread A: condition met, continuing work!\n");

    // Unlock the mutex after the condition is satisfied
    k_mutex_unlock(&mutex);
}

// Thread B: Sets the condition and signals Thread A
void thread_b_fn(void)
{
    // Simulate some delay before setting the condition
    k_sleep(K_SECONDS(2));

    // Lock the mutex before modifying the shared flag
    k_mutex_lock(&mutex, K_FOREVER);

    // Set the condition
    flag_ready = true;
    printk("Thread B: condition set, signaling thread A\n");

    // Signal the condition variable to wake up one waiting thread
    k_condvar_signal(&condvar);

    // Unlock the mutex
    k_mutex_unlock(&mutex);
}

// Define thread stacks and thread control blocks
K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);
struct k_thread thread_a_data;
struct k_thread thread_b_data;

// Main function: entry point
int main(void)
{
    printk("Condition variable demo started\n");

    // Create Thread A (waiter)
    k_thread_create(&thread_a_data, stack_a, STACK_SIZE,
                    (k_thread_entry_t)thread_a_fn,
                    NULL, NULL, NULL,
                    THREAD_A_PRIO, 0, K_NO_WAIT);

    // Create Thread B (signaler)
    k_thread_create(&thread_b_data, stack_b, STACK_SIZE,
                    (k_thread_entry_t)thread_b_fn,
                    NULL, NULL, NULL,
                    THREAD_B_PRIO, 0, K_NO_WAIT);

    return 0;
}
