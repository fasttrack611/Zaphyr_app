#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/sys/printk.h>   // For printing messages to the console
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>


#define STACK_SIZE 1024
#define PRIORITY 5

LOG_MODULE_REGISTER(main);

// Shared message queue for communication
K_MSGQ_DEFINE(sensor_q, sizeof(int), 10, 4);

// Thread stack areas
K_THREAD_STACK_DEFINE(collector_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(logger_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(controller_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(resilience_stack, STACK_SIZE);

// Thread control blocks
struct k_thread collector_thread, logger_thread, controller_thread, resilience_thread;

// Data Collector
void data_collector(void *a, void *b, void *c) {
    int sensor_data = 0;
    while (1) {
        sensor_data++;
        if (k_msgq_put(&sensor_q, &sensor_data, K_NO_WAIT) != 0) {
            LOG_WRN("Queue full, data dropped");
        }
        k_sleep(K_MSEC(100));
    }
}

// Data Logger
void data_logger(void *a, void *b, void *c) {
    int recv_data;
    while (1) {
        if (k_msgq_get(&sensor_q, &recv_data, K_FOREVER) == 0) {
            printk("Logged Data: %d\n", recv_data);
	    k_sleep(K_MSEC(50)); // inside data_logger
        }
    }
}

// Controller Thread
void controller_fn(void *a, void *b, void *c) {
    while (1) {
        size_t used = k_msgq_num_used_get(&sensor_q);
        LOG_INF("Controller: Queue usage = %d", used);

        // Simple control logic
        if (used > 8) {
            LOG_WRN("High queue usage, consider throttling");
        }

        k_sleep(K_SECONDS(1));
    }
}

// Resilience Monitor Thread
void resilience_fn(void *a, void *b, void *c) {
    while (1) {
        size_t usage = k_msgq_num_used_get(&sensor_q);
        if (usage == 10) {
            LOG_ERR("Resource exhaustion detected! Queue full.");
            k_msgq_purge(&sensor_q);
        }
        k_sleep(K_SECONDS(2));
    }
}

void main(void) {
    printk("Zephyr Multi-threaded Example\n");

    k_thread_create(&collector_thread, collector_stack, STACK_SIZE,
                    data_collector, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&logger_thread, logger_stack, STACK_SIZE,
                    data_logger, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&controller_thread, controller_stack, STACK_SIZE,
                    controller_fn, NULL, NULL, NULL,
                    PRIORITY + 1, 0, K_NO_WAIT);

    k_thread_create(&resilience_thread, resilience_stack, STACK_SIZE,
                    resilience_fn, NULL, NULL, NULL,
                    PRIORITY + 2, 0, K_NO_WAIT);
}

