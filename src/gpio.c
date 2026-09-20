#include "gpio.h"

#define pm5ctl0 (*(volatile unsigned char *)0x0130)
#define p1out (*(volatile unsigned char *)0x0202)
#define p1dir (*(volatile unsigned char *)0x0204)

#define led1 0x01
#define locklpm5 0x01

void gpio_led_init() {
	pm5ctl0 &= ~locklpm5;

	p1dir |= led1;
	p1out &= ~led1;
}

void gpio_led_on() { p1out |= led1; }

void gpio_led_off() { p1out &= ~led1; }
