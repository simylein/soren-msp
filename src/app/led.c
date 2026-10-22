#include "led.h"
#include "../lib/msp430fr5994.h"

#define led_red 0x08
#define led_green 0x04
#define led_blue 0x02

void led_init() {
	p8dir |= 0x0e;
	p8out |= 0x0e;
}

void led_red_on() { p8out &= ~led_red; }
void led_red_off() { p8out |= led_red; }
void led_green_on() { p8out &= ~led_green; }
void led_green_off() { p8out |= led_green; }
void led_blue_on() { p8out &= ~led_blue; }
void led_blue_off() { p8out |= led_blue; }
