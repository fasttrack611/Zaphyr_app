#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

// 📨 Define the message structure sent via message queue
struct message {
    uint32_t counter;
    char origin[16];  // Optional: sender ID for debugging
};

// 📬 Message queue declaration: up to 10 messages
K_MSGQ_DEFINE(my_msgq, sizeof(struct message), 10, 4);

// 🧵 Stack size and thread priorities
#define STACK_SIZE 1024
#define PRIORITY_PRODUCER 1
#define PRIORITY_CONSUMER 2

// 🧵 Thread stack and control blocks
K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);
struct k_thread producer_data;
struct k_thread consumer_data;

//  Producer thread: sends messages to the queue
void producer_thread(void *arg1, void *arg2, void *arg3)
{
    const char *name = (const char *)arg1;
    struct message msg = {0};

    while (1) {
        msg.counter++;
        strncpy(msg.origin, name, sizeof(msg.origin) - 1);

        if (k_msgq_put(&my_msgq, &msg, K_NO_WAIT) != 0) {
            printk("%s: Queue full! Purging...\n", name);
            k_msgq_purge(&my_msgq);
        } else {
            printk("%s: Sent counter value %d\n", name, msg.counter);
        }

        k_sleep(K_MSEC(500));
    }
}

//  Consumer thread: receives messages from the queue
void consumer_thread(void *arg1, void *arg2, void *arg3)
{
    const char *name = (const char *)arg1;
    struct message msg;

    while (1) {
        if (k_msgq_get(&my_msgq, &msg, K_FOREVER) == 0) {
            printk("%s: Received value %d from %s\n", name, msg.counter, msg.origin);
        }
    }
}

// 🧵 Main thread: creates and names the producer/consumer threads
void main(void)
{
    printk("\n In Main() ::  Producer-Consumer demo App\n");

    k_thread_create(&producer_data, producer_stack, STACK_SIZE,
                    producer_thread, "Producer-Thread", NULL, NULL,
                    PRIORITY_PRODUCER, 0, K_NO_WAIT);

    k_thread_create(&consumer_data, consumer_stack, STACK_SIZE,
                    consumer_thread, "Consumer-Thread", NULL, NULL,
                    PRIORITY_CONSUMER, 0, K_NO_WAIT);
}

