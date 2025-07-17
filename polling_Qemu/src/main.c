#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5
#define MSG_SIZE sizeof(uint32_t)
#define MSGQ_MAX_MSGS 5

K_SEM_DEFINE(my_sem, 0, 1);
K_MSGQ_DEFINE(my_msgq, MSG_SIZE, MSGQ_MAX_MSGS, 4);

void producer(void *a, void *b, void *c) {
    uint32_t msg = 42;  // Message to be sent via k_msgq

    while (1) {
        k_sleep(K_SECONDS(5));  // Wait before triggering event

        printk("Producer: giving semaphore\n");
        k_sem_give(&my_sem);    // Signal semaphore to make it available

        k_sleep(K_SECONDS(5));  // Simulate delay before next event

        printk("Producer: sending message\n");
        k_msgq_put(&my_msgq, &msg, K_NO_WAIT);  // Enqueue message into msgq (non-blocking)
    }
}

void consumer(void *a, void *b, void *c) {
    struct k_poll_event events[2];  // Poll two kernel objects

    // Event 0: Wait for semaphore to become available
    k_poll_event_init(&events[0], K_POLL_TYPE_SEM_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &my_sem);

    // Event 1: Wait for data to arrive in the message queue
    k_poll_event_init(&events[1], K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &my_msgq);

    uint32_t msg;  // Message buffer for msgq retrieval

    while (1) {
        // Wait for any event to become ready; blocks indefinitely
        int rc = k_poll(events, ARRAY_SIZE(events), K_FOREVER);

        if (rc == 0) {  // Poll success
            // Check if semaphore became available
            if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
                printk("Consumer: semaphore available\n");
                k_sem_take(&my_sem, K_NO_WAIT);  // Consume the semaphore (non-blocking)
            }

            // Check if message queue has data
            if (events[1].state == K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
                k_msgq_get(&my_msgq, &msg, K_NO_WAIT);  // Retrieve message (non-blocking)
                printk("Consumer: received message %d\n", msg);
            }

            // Reset polling state for both events before next loop
            events[0].state = K_POLL_STATE_NOT_READY;
            events[1].state = K_POLL_STATE_NOT_READY;
        } else {
            // If poll failed (shouldn't happen with K_FOREVER unless system error)
            printk("Consumer: poll error %d\n", rc);
        }
    }
}


K_THREAD_DEFINE(producer_id, STACK_SIZE, producer, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumer, NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void) {
    printk("Polling demo started\n");
    return 0;
}
