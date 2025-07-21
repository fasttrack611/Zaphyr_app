#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/pm/pm.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/pm.h>

#include <zephyr/pm/pm.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>


#define STACKSIZE 1024
#define THREAD_PRIORITY 7

LOG_MODULE_REGISTER(main);

// Thread 1: Display system time
void system_time_thread(void *thread_name, void *p2, void *p3)
{
    while (1) {
        uint64_t uptime = k_uptime_get();
        printk("Thread %s: System uptime: %llu ms\n", (char *)thread_name, uptime);
        k_sleep(K_SECONDS(18));
    }
}

// Thread 2: Collect system logs
void system_log_thread(void *thread_name, void *p2, void *p3)
{
   int count=40; 
    while (count--) {
        printk("Thread %s: Collecting system logs...\n", (char *)thread_name);
        log_process(); // Process and print pending log messages
        k_sleep(K_SECONDS(2));
    }
}

// Thread 3: Display system time
void system_time_thread3(void *thread_name, void *p2, void *p3)
{
    while (1) {
        uint64_t uptime = k_uptime_get();
        printk("Thread %s: System uptime: %llu ms\n", (char *)thread_name, uptime);
        k_sleep(K_SECONDS(1));
    }
}

#if 0 
// Power management thread (for demonstration)
void power_management_thread(void *p1, void *p2, void *p3)
{
    while (1) {
        printk("Power Management Thread: For demonstration...\n");
        k_sleep(K_SECONDS(10));
    }
}
#endif 



void power_management_thread(void *p1, void *p2, void *p3)
{
    const struct device *dev = DEVICE_DT_GET_ANY(zephyr_console);

    while (1) {
        uint64_t uptime = k_uptime_get();

        printk("PM Thread: System uptime: %llu ms\n", uptime);

        // Example: Suspend and resume a device
        if (dev && device_is_ready(dev)) {
            printk("PM Thread: Suspending console device...\n");
            int ret = pm_device_action_run(dev, PM_DEVICE_ACTION_SUSPEND);
            if (ret == 0) {
                printk("PM Thread: Device suspended.\n");
            } else {
                printk("PM Thread: Device suspend failed: %d\n", ret);
            }

            k_sleep(K_SECONDS(2));

            printk("PM Thread: Resuming console device...\n");
            ret = pm_device_action_run(dev, PM_DEVICE_ACTION_RESUME);
            if (ret == 0) {
                printk("PM Thread: Device resumed.\n");
            } else {
                printk("PM Thread: Device resume failed: %d\n", ret);
            }
        }

        // Optionally: Force system idle state for demonstration (not recommended for production)
         printk("PM Thread: Forcing system idle...\n");

        k_sleep(K_SECONDS(10));
    }
}

K_THREAD_DEFINE(system_time_thread1_id, STACKSIZE, system_time_thread, "T1", NULL, NULL, THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(system_log_thread2_id, STACKSIZE, system_log_thread, "T2", NULL, NULL, THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(system_time_thread3_id, STACKSIZE, system_time_thread3, "T3", NULL, NULL, THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(power_management_thread_id, STACKSIZE, power_management_thread, NULL, NULL, NULL, THREAD_PRIORITY+1, 0, 0);

void main(void)
{
    printk("Main thread is running\n");
    LOG_INF("System started!");
}
