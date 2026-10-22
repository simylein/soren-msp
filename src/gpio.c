#include "gpio.h"

#define pm5ctl0 (*(volatile unsigned char *)0x0130)
#define p1out (*(volatile unsigned char *)0x0202)
#define p1dir (*(volatile unsigned char *)0x0204)
#define p8out (*(volatile unsigned char *)0x0263)
#define p8dir (*(volatile unsigned char *)0x0265)

#define locklpm5 0x01

#define led_red 0x08
#define led_green 0x04
#define led_blue 0x02

void gpio_led_init() {
	pm5ctl0 &= ~locklpm5;

	p8dir |= 0x0e;
	p8out |= 0x0e;
}

void gpio_led_red_on() { p8out &= ~led_red; }
void gpio_led_red_off() { p8out |= led_red; }
void gpio_led_green_on() { p8out &= ~led_green; }
void gpio_led_green_off() { p8out |= led_green; }
void gpio_led_blue_on() { p8out &= ~led_blue; }
void gpio_led_blue_off() { p8out |= led_blue; }
