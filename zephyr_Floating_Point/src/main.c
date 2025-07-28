#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>  // For printf()

#define STACK_SIZE 1024
#define PRIORITY 5

K_THREAD_STACK_DEFINE(fp_stack, STACK_SIZE);
struct k_thread fp_thread_data;

void fp_thread(void *p1, void *p2, void *p3)
{
    float a = 3.14f;
    float b = 2.71f;
    float result1 = a * b;
    float result2 = a + b;

    printf("Floating point result-1: %.4f\n", result1);
    printf("Floating point result-2: %.4f\n", result2);
}

void main(void)
{
    printk("Main thread started\n");

    k_tid_t tid = k_thread_create(&fp_thread_data, fp_stack,
                                  STACK_SIZE,
                                  fp_thread,
                                  NULL, NULL, NULL,
                                  PRIORITY, 0, K_NO_WAIT);

    k_float_enable(tid, K_FP_REGS);

    printk("Floating point thread created\n");
}
