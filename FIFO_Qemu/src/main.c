#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define THREAD_PRIO 5

// Define FIFO statically
K_FIFO_DEFINE(my_fifo);

// Define data item structure
struct data_item_t {
    void *fifo_reserved;  // Required for FIFO linkage
    int value;
};

// Producer thread: generates data
void producer_fn(void *p1, void *p2, void *p3)
{
    int count = 0;
    while (1) {
        struct data_item_t *item = k_malloc(sizeof(struct data_item_t));
        item->value = count++;

        printk("Producer: sending %d\n", item->value);
        k_fifo_put(&my_fifo, item);

        k_sleep(K_MSEC(500));
    }
}

// Consumer thread: processes data
void consumer_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        struct data_item_t *item = k_fifo_get(&my_fifo, K_FOREVER);
        printk("Consumer: received %d\n", item->value);
        k_free(item);
    }
}

// Thread declarations
K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);
struct k_thread producer_data;
struct k_thread consumer_data;

int main(void)
{
    printk("FIFO demo started\n");

    k_thread_create(&producer_data, producer_stack, STACK_SIZE,
                    producer_fn, NULL, NULL, NULL,
                    THREAD_PRIO, 0, K_NO_WAIT);

    k_thread_create(&consumer_data, consumer_stack, STACK_SIZE,
                    consumer_fn, NULL, NULL, NULL,
                    THREAD_PRIO, 0, K_NO_WAIT);

    return 0;
}
