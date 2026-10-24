#pragma once

#include <stdint.h>

void led_init();

void led_red_on();
void led_red_off();
void led_green_on();
void led_green_off();
void led_blue_on();
void led_blue_off();

void led_red_blink(uint16_t on, uint16_t off);
void led_green_blink(uint16_t on, uint16_t off);
void led_blue_blink(uint16_t on, uint16_t off);
