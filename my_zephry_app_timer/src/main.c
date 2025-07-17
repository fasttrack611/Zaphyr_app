#include <zephyr/kernel.h>

#define TIMER_PERIOD_MS 1000
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

// Declare thread stack and control block
K_THREAD_STACK_DEFINE(my_stack_area, STACK_SIZE);

struct k_thread my_thread_data;

static struct k_timer my_timer;

// Thread function
void my_thread_fn(void *arg1, void *arg2, void *arg3)
{
    static int count=3;
    printk("Thread started after 5 seconds delay!\n");
    while ( count--) {
        printk("Thread is running...\n");
        k_sleep(K_SECONDS(12));
    }
}

void timer_expiry_function(struct k_timer *timer_id){
    uint64_t uptime = k_uptime_get();
    printk("Timer expired! System uptime: %llu ms\n", uptime);
}

void timer_stop_function(struct k_timer *timer_id)
{
    printk("Timer stopped.\n");
}

void main(void)
{
	printk("Starting Zephyr Timer Example\n");

	// Initialize the timer with expiry and stop functions
	k_timer_init(&my_timer, timer_expiry_function, timer_stop_function);

	// Start the timer: first expiration after TIMER_PERIOD_MS, then every TIMER_PERIOD_MS
	k_timer_start(&my_timer, K_MSEC(TIMER_PERIOD_MS), K_MSEC(TIMER_PERIOD_MS));

	k_sleep(K_SECONDS(15));
	k_timer_stop(&my_timer);


	printk("Main thread: sleeping for 5 seconds before starting new thread...\n");
	k_sleep(K_SECONDS(8));

	uint64_t start = k_uptime_get();
	k_sleep(K_SECONDS(8));
	uint64_t end = k_uptime_get();
	printk("Slept for: %llu ms\n", end - start);

	k_tid_t my_tid = k_thread_create(&my_thread_data, my_stack_area,
			K_THREAD_STACK_SIZEOF(my_stack_area),
			my_thread_fn,
			NULL, NULL, NULL,
			THREAD_PRIORITY, 0, K_NO_WAIT);

	printk("Main thread: new thread created.\n");
}

