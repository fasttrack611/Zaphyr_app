#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/arch/cpu.h>     // Provides arch_curr_cpu() to query current CPU ID

#define STACK_SIZE 1024
#define THREAD_PRIO 5
#define NUM_THREADS 4

// Thread function that logs its own execution context
void thread_fn(void *p1, void *p2, void *p3)
{
    const char *name = (const char *)p1;
    while (1) {
    //    uint32_t cpu_id = arch_curr_cpu()->id;
	uint32_t cpu_id = arch_get_current_cpu();  // If available via arch API
        printk("%s running on CPU %u\n", name, cpu_id);
        k_sleep(K_MSEC(500));
    }
}

// Thread declarations
K_THREAD_STACK_DEFINE(thread_stack[NUM_THREADS], STACK_SIZE);
struct k_thread thread_data[NUM_THREADS];

int main(void)
{
    printk("Zephyr SMP demo started\n");
	char *name;
	
    for (int i = 0; i < NUM_THREADS; ++i) {
//        char *name = k_malloc(16);
        snprintk(name, 16, "Thread-%d", i);

        k_thread_create(&thread_data[i],
                        thread_stack[i], STACK_SIZE,
                        thread_fn,
                        name, NULL, NULL,
                        THREAD_PRIO, 0, K_NO_WAIT);

        k_thread_name_set(&thread_data[i], name);
    }

    return 0;
}
