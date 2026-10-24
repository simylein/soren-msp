#include "app/led.h"
#include "app/timer.h"
#include "lib/msp430fr5994.h"

#define locklpm5 0x01

int main() {
	__asm__ volatile("nop");
	__asm__ volatile("eint");
	__asm__ volatile("nop");

	pm5ctl0 &= ~locklpm5;

	timer_init();
	led_init();

	led_green_blink(200, 100);
	led_green_blink(200, 100);
	led_green_blink(200, 100);
	led_green_blink(200, 100);

	timer_sleep_s(1);

	while (1) {
		led_red_blink(200, 100);
		led_green_blink(200, 100);
		led_blue_blink(200, 100);

		timer_sleep_s(4);
	}
}
