#include "app/led.h"
#include "app/msp430.h"
#include "app/timer.h"

int main(void) {
	timer_init();
	led_init();

	msp430_gpio_unlock();
	msp430_interrupts_enable();

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
