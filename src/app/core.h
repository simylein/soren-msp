#pragma once

#include "config.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct uplink_t {
	uint8_t kind;
	uint8_t data[64];
	uint8_t data_len;
} uplink_t;

int configure(config_t *config);

int transceive(config_t *config, uplink_t *uplink);
