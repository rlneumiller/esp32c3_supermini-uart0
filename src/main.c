#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>
#include <string.h>

#define UART0_NODE DT_NODELABEL(uart0)

static const struct device *uart_dev = DEVICE_DT_GET(UART0_NODE);
static const char *test_str = "This is a test string sent over uart.\n";

#define RING_BUF_SIZE 2048
RING_BUF_DECLARE(tx_ring_buf, RING_BUF_SIZE);

static void uart_cb(const struct device *dev, void *user_data)
{
	uint8_t c;
	if (ring_buf_get(&tx_ring_buf, &c, 1) > 0) {
		uart_fifo_fill(dev, &c, 1);
	} else {
		uart_irq_tx_disable(dev);
	}
}

void send_gcode(const char *cmd)
{
	size_t len = strlen(cmd);
	ring_buf_put(&tx_ring_buf, cmd, len);
	uart_irq_tx_enable(uart_dev);
}

int main(void)
{
	if (!device_is_ready(uart_dev)) {
		printk("Device not ready\n");
		return -1;
	}

	uart_irq_callback_set(uart_dev, uart_cb);
	uart_irq_tx_enable(uart_dev);

	while (1) {
		// for (int i = 0; test_str[i] != '\0'; i++) {
		// 	uart_poll_out(uart_dev, test_str[i]);
		// }

		send_gcode("G1 X100 F6000\n");
		printf("Sent gcode G1 X100 F6000\n");

		k_msleep(500);
	}
	return 0;
}