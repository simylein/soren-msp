#include "gpio.h"
#include "timer.h"

int main() {
	gpio_led_init();
	timer_init();

	__asm__ volatile("nop");
	__asm__ volatile("eint");
	__asm__ volatile("nop");

	while (1) {
		gpio_led_on();
		timer_sleep_s(1);
		gpio_led_off();
		timer_sleep_s(1);
	}
}
