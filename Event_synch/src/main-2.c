#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define PERIOD_MS 1000

// 🧪 Timer handler (acts like an ISR)
void timer_callback(struct k_timer *timer) {
    printk("\n  Timer fired! ISR simulated\n");
}

// 🔧 Declare and initialize timer
K_TIMER_DEFINE(my_timer, timer_callback, NULL);

void main(void) {
    printk("\n TIMER:: Timer ISR demo started\n");

    // Start periodic timer
    k_timer_start(&my_timer, K_MSEC(PERIOD_MS), K_MSEC(PERIOD_MS));

    while (1) {
        k_sleep(K_SECONDS(5));
        printk("\n Main()::  Main thread alive...\n");
    }
}


