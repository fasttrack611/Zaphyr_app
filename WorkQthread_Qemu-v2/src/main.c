#include <zephyr/kernel.h>
//#include <zephyr/sys/printk.h>
#define STACK_SIZE 1024
#define PRIORITY -4

//#include <device.h>
#include <zephyr/drivers/sensor.h>
#define SENSOR_NODE DT_NODELABEL(hts221)

void main(void)
{
    const struct device *sensor = DEVICE_DT_GET(SENSOR_NODE);

    if (!device_is_ready(sensor)) {
        printk("Sensor device not ready\n");
        return;
    }

    printk("HTS221 sensor ready\n");

    while (1) {
        struct sensor_value temp, humidity;
        int ret = sensor_sample_fetch(sensor);
        if (ret < 0) {
            printk("Sample fetch error: %d\n", ret);
            k_sleep(K_MSEC(1000));
            continue;
        }

        ret = sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        if (ret < 0) {
            printk("Temp channel error: %d\n", ret);
        } else {
            printk("Temperature: %d.%06d C\n", temp.val1, temp.val2);
        }

        ret = sensor_channel_get(sensor, SENSOR_CHAN_HUMIDITY, &humidity);
        if (ret < 0) {
            printk("Humidity channel error: %d\n", ret);
        } else {
            printk("Humidity: %d.%06d %%\n", humidity.val1, humidity.val2);
        }

        k_sleep(K_MSEC(2000));
    }
}
