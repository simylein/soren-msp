#pragma once

#include <stdint.h>

void sx1278_init(void);

uint8_t sx1278_version(void);

int sx1278_lora(void);
int sx1278_sleep(void);
int sx1278_standby(void);
