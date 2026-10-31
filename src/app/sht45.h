#pragma once

#include <stdint.h>

void sht45_init(void);

int sht45_read(uint16_t *temperature, uint16_t *humidity);
