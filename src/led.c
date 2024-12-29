#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <stdio.h>
#include "led.h"

#define LED_SLEEP_TIME_MS 1000 // Sleep for 1 second
#define LED0_NODE         DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
#define STACK_SIZE_LED_THREAD 1024
#define PRIORITY_LED_THREAD   10

// Timing constants in milliseconds
#define HEARTBEAT_PULSE 100
#define BETWEEN_BEATS   700
#define NUM_BEATS       10

void led_thread_proc(void)
{
	printf("Entering LED thread proc\n");

	int ret;

	if (!gpio_is_ready_dt(&led)) {
		printf("Error: LED GPIO port not ready\n");
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printf("Error configuring LED GPIO pin\n");
		return;
	}

	while (1) {
		// First beat
		gpio_pin_set_dt(&led, 1);
		k_sleep(K_MSEC(HEARTBEAT_PULSE));
		gpio_pin_set_dt(&led, 0);
		k_sleep(K_MSEC(HEARTBEAT_PULSE));

		// Second beat
		gpio_pin_set_dt(&led, 1);
		k_sleep(K_MSEC(HEARTBEAT_PULSE));
		gpio_pin_set_dt(&led, 0);

		// Pause between beats
		k_sleep(K_MSEC(BETWEEN_BEATS));
	}
}

K_THREAD_DEFINE(toggle_led_tid, STACK_SIZE_LED_THREAD, led_thread_proc, NULL, NULL, NULL,
		PRIORITY_LED_THREAD, 0, 0);