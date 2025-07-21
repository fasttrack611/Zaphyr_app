#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// Thread stack size and priorities
#define STACK_SIZE 1024
#define THREAD_A_PRIO 5
#define THREAD_B_PRIO 5

// Declare shared synchronization primitives
K_MUTEX_DEFINE(mutex);              // Protects access to flag_ready
K_CONDVAR_DEFINE(condvar);          // Condition variable for signaling

volatile bool flag_ready = false;   // Shared condition flag

// Thread A: waits until flag_ready becomes true
void thread_a_fn(void)
{
    k_mutex_lock(&mutex, K_FOREVER);

    while (!flag_ready) {
        printk("Thread A: waiting for condition...\n");

        // Releases mutex and sleeps until signaled
        k_condvar_wait(&condvar, &mutex, K_FOREVER);
    }

    printk("Thread A: condition met, continuing work!\n");

    k_mutex_unlock(&mutex);
}

// Thread B: sets the condition and signals thread A
void thread_b_fn(void)
{
    k_sleep(K_SECONDS(2));  // Simulate some delay

    k_mutex_lock(&mutex, K_FOREVER);

    flag_ready = true;
    printk("Thread B: condition set, signaling thread A\n");

    k_condvar_signal(&condvar);  // Wake one waiting thread

    k_mutex_unlock(&mutex);
}

// Define threads
K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);
struct k_thread thread_a_data;
struct k_thread thread_b_data;

int main(void)
{
    printk("Condition variable demo started\n");

    // Create both threads
    k_thread_create(&thread_a_data, stack_a, STACK_SIZE,
                    (k_thread_entry_t)thread_a_fn,
                    NULL, NULL, NULL,
                    THREAD_A_PRIO, 0, K_NO_WAIT);

    k_thread_create(&thread_b_data, stack_b, STACK_SIZE,
                    (k_thread_entry_t)thread_b_fn,
                    NULL, NULL, NULL,
                    THREAD_B_PRIO, 0, K_NO_WAIT);

    return 0;
}
