#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5

K_SEM_DEFINE(data_ready_sem, 0, 1);     // Binary semaphore
K_MUTEX_DEFINE(data_mutex);            // Mutex for shared data

int shared_data = 0;

void producer(void *a, void *b, void *c) {
    while (1) {
        k_mutex_lock(&data_mutex, K_FOREVER);
        shared_data++;
        printk("Producer: updated shared_data = %d\n", shared_data);
        k_mutex_unlock(&data_mutex);

        k_sem_give(&data_ready_sem);  // Signal consumer
        k_sleep(K_SECONDS(2));
    }
}

void consumer(void *a, void *b, void *c) {
    while (1) {
        printk("Consumer: waiting for data\n");
        k_sem_take(&data_ready_sem, K_FOREVER);

        k_mutex_lock(&data_mutex, K_FOREVER);
        printk("Consumer: read shared_data = %d\n", shared_data);
        k_mutex_unlock(&data_mutex);
    }
}

K_THREAD_DEFINE(producer_id, STACK_SIZE, producer, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumer, NULL, NULL, NULL, PRIORITY, 0, 0);

void main(void) {
    
	printk(" Main Thread Running..\n");
}
