#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

void smp_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        printk("Hello from CPU %d\n", arch_curr_cpu());
        k_sleep(K_MSEC(1000));
    }
}

K_THREAD_DEFINE(thread0, 1024, smp_thread_entry, NULL, NULL, NULL,
                K_PRIO_COOP(7), 0, 0);
K_THREAD_DEFINE(thread1, 1024, smp_thread_entry, NULL, NULL, NULL,
                K_PRIO_COOP(7), 0, 0);

void main(void)
{
	printk("SMP demo started\n");
}
