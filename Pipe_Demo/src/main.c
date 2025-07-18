#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

K_PIPE_DEFINE(my_pipe, 1024, 4);

#define MSG_SIZE 10
#define NUM_MSGS 5

void producer_thread(void)
{
    char message[MSG_SIZE];
    size_t bytes_written;

    for (int i = 0; i < NUM_MSGS; i++) {
        snprintk(message, MSG_SIZE, "Message %d", i);
        k_pipe_put(&my_pipe, message, MSG_SIZE, &bytes_written, MSG_SIZE, K_MSEC(100));
        printk("Producer sent: %s\n", message);
        k_sleep(K_MSEC(500));
    }
}

void consumer_thread(void)
{
    char buffer[MSG_SIZE];
    size_t bytes_read;

    for (int i = 0; i < NUM_MSGS; i++) {
        k_pipe_get(&my_pipe, buffer, MSG_SIZE, &bytes_read, 0, K_FOREVER);
        printk("Consumer received: %s\n", buffer);
    }
}

K_THREAD_DEFINE(producer, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(consumer, 1024, consumer_thread, NULL, NULL, NULL, 5, 0, 0);

void main(void)
{
	printk("Pipe communication demo started\n");
}
