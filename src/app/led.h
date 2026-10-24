#pragma once

#include <stdint.h>

void led_init(void);

void led_red_on(void);
void led_red_off(void);
void led_green_on(void);
void led_green_off(void);
void led_blue_on(void);
void led_blue_off(void);

void led_red_blink(uint16_t on, uint16_t off);
void led_green_blink(uint16_t on, uint16_t off);
void led_blue_blink(uint16_t on, uint16_t off);

void led_byte_blink(uint8_t value);
