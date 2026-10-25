#pragma once

#include <stdint.h>

void adc49_init(void);

uint16_t adc49_photovoltaic(void);
uint16_t adc49_battery(void);
