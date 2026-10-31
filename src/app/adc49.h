#pragma once

#include <stdint.h>

void adc49_init(void);

int adc49_read(uint16_t *photovoltaic, uint16_t *battery);
