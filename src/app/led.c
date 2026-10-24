#include "led.h"
#include "../lib/msp430fr5994.h"
#include "timer.h"
#include <stdint.h>

#define led_red 0x08
#define led_green 0x04
#define led_blue 0x02

void led_init(void) {
	p8dir |= 0x0e;
	p8out |= 0x0e;
}

void led_red_on(void) { p8out &= ~led_red; }
void led_red_off(void) { p8out |= led_red; }
void led_green_on(void) { p8out &= ~led_green; }
void led_green_off(void) { p8out |= led_green; }
void led_blue_on(void) { p8out &= ~led_blue; }
void led_blue_off(void) { p8out |= led_blue; }

void led_red_blink(uint16_t on, uint16_t off) {
	led_red_on();
	timer_sleep_ms(on);
	led_red_off();
	timer_sleep_ms(off);
}

void led_green_blink(uint16_t on, uint16_t off) {
	led_green_on();
	timer_sleep_ms(on);
	led_green_off();
	timer_sleep_ms(off);
}

void led_blue_blink(uint16_t on, uint16_t off) {
	led_blue_on();
	timer_sleep_ms(on);
	led_blue_off();
	timer_sleep_ms(off);
}
