#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/arch/cpu.h>
//#include <zephyr/arch/x86/exc.h>
#include <zephyr/init.h>
#include <zephyr/irq_offload.h>
//#include <zephyr/sem.h>
//#include <zephyr/msgq.h>

K_THREAD_STACK_DEFINE(user_stack, 1024);
struct k_thread user_thread_data;

K_SEM_DEFINE(my_sem, 0, 1);
K_MSGQ_DEFINE(my_msgq, sizeof(int), 1, 4);

void cause_invalid_memory_access(void) {
    volatile int *ptr = (int *)0xFFFFFFFF;
    *ptr = 42;
}

void cause_stack_overflow(void) {
    char buffer[1024];
    buffer[0] = 'A';
    cause_stack_overflow();
}

void cause_assertion_failure(void) {
    int x = 0;
    __ASSERT(x == 0, "Assertion failed: x is zero");
}

void cause_unhandled_exception(void) {
    int x = 0;
    int y = 1 / x;
    printk("Result: %d\n", y);
}

void cause_kernel_bug(void) {
    k_sem_init(NULL, 0, 1);
}

void user_thread(void *p1, void *p2, void *p3) {
    printk("User thread started\n");

    // Uncomment one of the following to trigger a fatal error:
     cause_invalid_memory_access();
    //cause_stack_overflow();
     //cause_assertion_failure();
    // cause_unhandled_exception();
    // cause_kernel_bug();

    int val = 123;
    k_msgq_put(&my_msgq, &val, K_NO_WAIT);
    k_sem_give(&my_sem);
}

void main(void) {
    printk("Zephyr Fatal Error Sample\n");

    k_thread_create(&user_thread_data, user_stack,
                    K_THREAD_STACK_SIZEOF(user_stack),
                    user_thread,
                    NULL, NULL, NULL,
                    1, K_USER, K_NO_WAIT);

    k_sem_take(&my_sem, K_FOREVER);
    int val;
    k_msgq_get(&my_msgq, &val, K_FOREVER);
    printk("Received from msgq: %d\n", val);
}

void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf) {
    printk("Custom fatal error handler invoked!\n");
    printk("Fatal error reason code: %d\n", reason);
    k_fatal_halt(reason);
}
