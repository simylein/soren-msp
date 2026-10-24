#include "app/core.h"
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

	config_t config;
	config.frame = 0;

	config.id[0] = 0xbe;
	config.id[1] = 0xef;
	config.firmware[0] = 0x00;
	config.firmware[1] = 0x20;
	config.hardware[0] = 0x00;
	config.hardware[1] = 0x30;
	config.led_red = true;
	config.led_green = true;
	config.led_blue = true;
	config.frequency = 433225000;
	config.bandwidth = 125000;
	config.coding_rate = 5;
	config.spreading_factor = 7;
	config.preamble_length = 6;
	config.checksum = true;
	config.tx_power = 2;
	config.sync_word = 0x26;

	if (sx1278_lora() == -1) {
		led_red_blink(200, 100);
	}
	if (sx1278_standby() == -1) {
		led_red_blink(200, 100);
	}
	if (sx1278_fifo() == -1) {
		led_red_blink(200, 100);
	}
	if (configure(&config) == -1) {
		led_red_blink(200, 100);
	}
	if (sx1278_sleep() == -1) {
		led_red_blink(200, 100);
	}

	while (true) {
		if (sx1278_standby() == -1) {
			led_red_blink(200, 100);
		}
		if (transceive(&config) == -1) {
			led_red_blink(200, 100);
		}
		if (sx1278_sleep() == -1) {
			led_red_blink(200, 100);
		}

		timer_sleep_s(8);
	}
}
