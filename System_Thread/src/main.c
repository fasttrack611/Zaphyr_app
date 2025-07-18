#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread.h>

#define STACK_SIZE 512
#define PRIORITY1 1
#define PRIORITY2 2

K_THREAD_STACK_DEFINE(stack_area1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area2, STACK_SIZE);
struct k_thread thread_data1;
struct k_thread thread_data2;

void thread_func(void *p1, void *p2, void *p3) {
int count=5; 
	while (count--) {
        printk("Thread %s running\n", (char *)p1);
        k_sleep(K_MSEC(1000));
    }
}

void print_thread_info(const struct k_thread *thread, void *user_data) {
    const char *name = k_thread_name_get((k_tid_t)thread);
    int prio = thread->base.prio;
    size_t stack_size = thread->stack_info.size;

    printk("Thread: %-12s | Priority: %2d | Stack size: %4zu bytes\n",
           name ? name : "Unnamed",
           prio,
           stack_size);
}



void main(void) {
    printk("\n Listing all threads (including system threads):\n\n");

    k_thread_create(&thread_data1, stack_area1, STACK_SIZE,
                    thread_func, "T1", NULL, NULL,
                    PRIORITY1, 0, K_NO_WAIT);
    k_thread_name_set(&thread_data1, "T1");

    k_thread_create(&thread_data2, stack_area2, STACK_SIZE,
                    thread_func, "T2", NULL, NULL,
                    PRIORITY2, 0, K_NO_WAIT);
    k_thread_name_set(&thread_data2, "T2");

    k_thread_foreach(print_thread_info, NULL);
}

#if 0 
void main(void) {
    printk("\n Listing all threads (including system threads):\n\n");

    k_thread_create(&thread_data1, stack_area1, STACK_SIZE,
                    thread_func, "T1", NULL, NULL,
                    PRIORITY1, 0, K_NO_WAIT);

    k_thread_create(&thread_data2, stack_area2, STACK_SIZE,
                    thread_func, "T2", NULL, NULL,
                    PRIORITY2, 0, K_NO_WAIT);

    k_thread_foreach(print_thread_info, NULL);
}
#endif 
