#include "gpio.h"

int main() {
	gpio_led_init();
	gpio_led_on();

	while (1) {
	}
}
