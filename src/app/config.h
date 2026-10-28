#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct config_t {
	uint8_t id[2];
	uint8_t key[16];
	uint16_t frame;
	uint8_t firmware[2];
	uint8_t hardware[2];
	bool led_red;
	bool led_green;
	bool led_blue;
	uint32_t frequency;
	uint32_t bandwidth;
	uint8_t coding_rate;
	uint8_t spreading_factor;
	uint8_t preamble_length;
	bool checksum;
	uint8_t tx_power;
	uint8_t sync_word;
} config_t;

void config_read(config_t *config);
void config_write(config_t *config);
int config_validate(config_t *config);
