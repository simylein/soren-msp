#include "app/led.h"
#include "app/timer.h"
#include "lib/msp430fr5994.h"

#define locklpm5 0x01

int main() {
	pm5ctl0 &= ~locklpm5;

	timer_init();
	led_init();

	__asm__ volatile("nop");
	__asm__ volatile("eint");
	__asm__ volatile("nop");

	while (1) {
		led_red_on();
		timer_sleep_ms(200);
		led_red_off();
		timer_sleep_ms(100);

		led_green_on();
		timer_sleep_ms(200);
		led_green_off();
		timer_sleep_ms(100);

		led_blue_on();
		timer_sleep_ms(200);
		led_blue_off();
		timer_sleep_ms(100);

		timer_sleep_s(4);
	}
}
