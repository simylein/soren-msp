#pragma once

#include <stdint.h>

void timer_init(void);

void timer_sleep_s(uint8_t seconds);
void timer_sleep_ms(uint16_t milliseconds);
