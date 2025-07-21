#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>

#define STACKSIZE        1024
#define THREAD_PRIORITY  7  // Lower number = higher priority

LOG_MODULE_REGISTER(main);

/*
 * 🧵 Thread 1: Periodically print system uptime (every 18 seconds)
 * Simulates long-interval status tracking
 */
void system_time_thread(void *thread_name, void *p2, void *p3)
{
    while (1) {
        uint64_t uptime = k_uptime_get();
        printk("🕒 Thread %s: System uptime: %llu ms\n", (char *)thread_name, uptime);
        k_sleep(K_SECONDS(18));
    }
}

/*
 * 🧵 Thread 2: Collect and flush system logs for 40 iterations
 * Demonstrates deferred logging via log_process()
 */
void system_log_thread(void *thread_name, void *p2, void *p3)
{
    int count = 40;
    while (count--) {
        printk("📦 Thread %s: Collecting system logs...\n", (char *)thread_name);
        log_process();  // Flush pending log messages
        k_sleep(K_SECONDS(2));
    }
}

/*
 * 🧵 Thread 3: Print uptime every second
 * Useful for high-frequency timestamp logging
 */
void system_time_thread3(void *thread_name, void *p2, void *p3)
{
    while (1) {
        uint64_t uptime = k_uptime_get();
        printk("📆 Thread %s: System uptime: %llu ms\n", (char *)thread_name, uptime);
        k_sleep(K_SECONDS(1));
    }
}

/*
 * 🧵 Thread 4: Demonstrates power management APIs
 * Suspends and resumes console device every 12 seconds
 */
void power_management_thread(void *p1, void *p2, void *p3)
{
    const struct device *dev = DEVICE_DT_GET_ANY(zephyr_console);

    while (1) {
        uint64_t uptime = k_uptime_get();
        printk("⚡ PM Thread: Uptime: %llu ms\n", uptime);

        if (dev && device_is_ready(dev)) {
            printk("⚙️ PM Thread: Suspending console device...\n");
            int ret = pm_device_action_run(dev, PM_DEVICE_ACTION_SUSPEND);
            printk("➡️ PM Thread: %s\n", ret == 0 ? "Device suspended" : "Device suspend failed");

            k_sleep(K_SECONDS(2));

            printk("⚙️ PM Thread: Resuming console device...\n");
            ret = pm_device_action_run(dev, PM_DEVICE_ACTION_RESUME);
            printk("⬅️ PM Thread: %s\n", ret == 0 ? "Device resumed" : "Device resume failed");
        }

        printk("🔕 PM Thread: Sleeping to simulate idle...\n");
        k_sleep(K_SECONDS(10));
    }
}

/* 🧵 Thread declarations with unique names and priorities */
K_THREAD_DEFINE(system_time_thread1_id, STACKSIZE, system_time_thread,
                "T1", NULL, NULL, THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(system_log_thread2_id, STACKSIZE, system_log_thread,
                "T2", NULL, NULL, THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(system_time_thread3_id, STACKSIZE, system_time_thread3,
                "T3", NULL, NULL, THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(power_management_thread_id, STACKSIZE, power_management_thread,
                NULL, NULL, NULL, THREAD_PRIORITY + 1, 0, 0);

/*
 * 🧵 Main thread: starts the system and logs boot status
 */
void main(void)
{
    printk("✅ Main thread is running\n");
    LOG_INF("🌟 System boot completed");
}
