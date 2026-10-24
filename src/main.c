#include "app/led.h"
#include "app/msp430.h"
#include "app/sx1278.h"
#include "app/timer.h"
#include <stdbool.h>
#include <stdint.h>

int main(void) {
	timer_init();
	led_init();
	sx1278_init();

	msp430_gpio_unlock();
	msp430_interrupts_enable();

	if (sx1278_sleep() == 0) {
		led_green_blink(200, 100);
	} else {
		led_red_blink(200, 100);
	}

	if (sx1278_version() == 0x12) {
		led_green_blink(200, 100);
	} else {
		led_red_blink(200, 100);
	}

	timer_sleep_s(1);

	while (true) {
		if (sx1278_lora() == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_standby() == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_fifo() == -1) {
			led_red_blink(500, 500);
		}

		if (sx1278_frequency(433225000) == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_bandwidth(125000) == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_spreading_factor(7) == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_coding_rate(5) == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_checksum(true) == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_tx_power(2) == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_sync_word(0x26) == -1) {
			led_red_blink(500, 500);
		}

		uint8_t tx_data[6];
		uint8_t tx_data_len = 6;
		tx_data[0] = 0xbe;
		tx_data[1] = 0xef;
		tx_data[2] = 0x00;
		tx_data[3] = 0x00;
		tx_data[4] = 0x00;
		tx_data[5] = 0x00;
		if (sx1278_transmit(tx_data, tx_data_len, 512) == -1) {
			led_red_blink(500, 500);
		}

		if (sx1278_sleep() == -1) {
			led_red_blink(500, 500);
		}
		timer_sleep_s(8);
	}
}
