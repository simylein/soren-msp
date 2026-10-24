#include "app/led.h"
#include "app/msp430.h"
#include "app/sx1278.h"
#include "app/timer.h"
#include <stdint.h>

void blink_byte(uint8_t value) {
	for (uint8_t i = 0; i < 8; i++) {
		if (value & 0x80) {
			led_green_on();
		} else {
			led_red_on();
		}

		timer_sleep_ms(400);

		led_red_off();
		led_green_off();

		timer_sleep_ms(250);

		value <<= 1;
	}
}

int main(void) {
	timer_init();
	led_init();
	sx1278_init();
	sx1278_sleep();

	msp430_gpio_unlock();
	msp430_interrupts_enable();

	if (sx1278_version() == 0x12) {
		led_green_blink(200, 100);
	} else {
		led_red_blink(200, 100);
	}

	timer_sleep_s(1);

	while (1) {
		if (sx1278_lora() == -1) {
			led_red_blink(500, 500);
		}
		if (sx1278_standby() == -1) {
			led_red_blink(500, 500);
		}

		if (sx1278_sleep() == -1) {
			led_red_blink(500, 500);
		}
		timer_sleep_s(8);
	}
}
