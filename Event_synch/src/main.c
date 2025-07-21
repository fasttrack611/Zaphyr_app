#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// 🧵 Stack size and thread priorities
#define STACK_SIZE 1024
#define THREAD_PRIO 5

// 🏁 Define the shared event object (bitwise flags)
K_EVENT_DEFINE(my_event);

// 🔢 Define bits each thread will wait for
#define EVENT_BIT_A 0x01  // Bit 0
#define EVENT_BIT_B 0x02  // Bit 1
#define EVENT_BIT_C 0x03  // Bit 0 + Bit 1

// Thread A: waits for EVENT_BIT_A
void thread_a_fn(void)
{
    printk("Thread A: waiting for bit 0...\n");

    // Wait for bit 0 to be posted
    uint32_t result = k_event_wait(&my_event, EVENT_BIT_A, false, K_FOREVER);

    if (result & EVENT_BIT_A) {
        printk("Thread A: bit 0 received!\n");
    }
}

// Thread B: waits for EVENT_BIT_B
void thread_b_fn(void)
{
    printk("Thread B: waiting for bit 1...\n");

    // Wait for bit 1 to be posted
    uint32_t result = k_event_wait(&my_event, EVENT_BIT_B, false, K_FOREVER);

    if (result & EVENT_BIT_B) {
        printk("Thread B: bit 1 received!\n");
    }
}

// Thread C: waits for BOTH bits (bit 0 AND bit 1)
void thread_c_fn(void)
{
    printk("Thread C: waiting for bits 0 AND 1...\n");

    // Wait until both bits are posted
    uint32_t result = k_event_wait_all(&my_event, EVENT_BIT_C, false, K_FOREVER);

    if ((result & EVENT_BIT_C) == EVENT_BIT_C) {
        printk("Thread C: both bits received!\n");
    }
}

// Declare stacks and thread control blocks
K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_c, STACK_SIZE);

struct k_thread thread_a_data;
struct k_thread thread_b_data;
struct k_thread thread_c_data;

int main(void)
{
    printk("Event synchronization demo started\n");

    //  Start threads
    k_thread_create(&thread_a_data, stack_a, STACK_SIZE,
                    thread_a_fn, NULL, NULL, NULL,
                    THREAD_PRIO, 0, K_NO_WAIT);

    k_thread_create(&thread_b_data, stack_b, STACK_SIZE,
                    thread_b_fn, NULL, NULL, NULL,
                    THREAD_PRIO, 0, K_NO_WAIT);

    k_thread_create(&thread_c_data, stack_c, STACK_SIZE,
                    thread_c_fn, NULL, NULL, NULL,
                    THREAD_PRIO, 0, K_NO_WAIT);

    //  Simulate event posting after some delay
    k_sleep(K_SECONDS(1));
    printk("Main: posting bit 0\n");
    k_event_post(&my_event, EVENT_BIT_A);  // Wakes thread A

    k_sleep(K_SECONDS(1));
    printk("Main: posting bit 1\n");
    k_event_post(&my_event, EVENT_BIT_B);  // Wakes thread B and thread C

    return 0;
}
