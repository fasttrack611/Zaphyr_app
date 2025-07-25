#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// Define stack size and thread priority
#define STACK_SIZE 1024
#define PRIORITY 5

// Define a binary semaphore (initial count = 0, max count = 1)
// Used to signal the consumer when new data is available
K_SEM_DEFINE(data_ready_sem, 0, 1);

// Define a mutex to protect access to shared data
K_MUTEX_DEFINE(data_mutex);

// Shared resource between producer and consumer
int shared_data = 0;

/**
 * Producer thread function
 * - Locks the mutex to safely update shared_data
 * - Signals the consumer using the semaphore
 * - Sleeps for 2 seconds before producing again
 */
void producer(void *a, void *b, void *c) {
    while (1) {
        // Lock the mutex before accessing shared_data
        k_mutex_lock(&data_mutex, K_FOREVER);

        // Update shared data
        shared_data++;
        printk("Producer: updated shared_data = %d\n", shared_data);

        // Unlock the mutex after update
        k_mutex_unlock(&data_mutex);

        // Signal the consumer that new data is ready
        k_sem_give(&data_ready_sem);

        // Sleep for 2 seconds before producing again
        k_sleep(K_SECONDS(2));
    }
}

/**
 * Consumer thread function
 * - Waits for the semaphore signal from producer
 * - Locks the mutex to safely read shared_data
 */
void consumer(void *a, void *b, void *c) {
    while (1) {
        // Wait for producer to signal that data is ready
        printk("Consumer: waiting for data\n");
        k_sem_take(&data_ready_sem, K_FOREVER);

        // Lock the mutex before reading shared_data
        k_mutex_lock(&data_mutex, K_FOREVER);

        // Read and print the shared data
        printk("Consumer: read shared_data = %d\n", shared_data);

        // Unlock the mutex after reading
        k_mutex_unlock(&data_mutex);
    }
}

// Define producer thread at build time
K_THREAD_DEFINE(producer_id, STACK_SIZE, producer, NULL, NULL, NULL,
                PRIORITY, 0, 0);

// Define consumer thread at build time
K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumer, NULL, NULL, NULL,
                PRIORITY, 0, 0);

/**
 * Main thread function
 * - Runs after kernel initialization
 * - Can be used for additional setup or logging
 */
void main(void) {
    printk("Main Thread Running..\n");
}
