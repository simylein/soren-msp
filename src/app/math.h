#pragma once

#include <stdint.h>

typedef union uint32_split_t {
	uint32_t value;
	uint16_t words[2];
	uint8_t bytes[4];
} uint32_split_t;

uint16_t min16(uint16_t alpha, uint16_t bravo);
uint16_t sub16(uint16_t alpha, uint16_t bravo);

uint32_t divide_15625(uint32_t value, uint16_t *rem);
uint32_t calculate_frf(uint32_t frequency);
