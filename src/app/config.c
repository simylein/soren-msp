#include "config.h"
#include "../lib/msp430fr5994.h"
#include <stdbool.h>
#include <stdint.h>

bool config_read_bool(uint8_t *index) { return (&framcfg)[(*index)++]; }

uint8_t config_read_uint8(uint8_t *index) { return (&framcfg)[(*index)++]; }

uint16_t config_read_uint16(uint8_t *index) {
	uint16_t value = 0;

	for (uint8_t i = 0; i < sizeof(uint16_t); i++) {
		value = (value << 8) | (&framcfg)[(*index)++];
	}

	return value;
}

uint32_t config_read_uint32(uint8_t *index) {
	uint16_t high = config_read_uint16(index);
	uint16_t low = config_read_uint16(index);

	return ((uint32_t)high << 16) | low;
}

void config_read_blob(uint8_t *val, uint8_t len, uint8_t *index) {
	for (uint8_t ind = 0; ind < len; ind++) {
		val[ind] = (&framcfg)[(*index)++];
	}
}

void config_write_bool(bool val, uint8_t *index) { (&framcfg)[(*index)++] = val; }

void config_write_uint8(uint8_t val, uint8_t *index) { (&framcfg)[(*index)++] = val; }

void config_write_uint16(uint16_t val, uint8_t *index) {
	(&framcfg)[(*index)++] = val >> 8;
	(&framcfg)[(*index)++] = val >> 0;
}

void config_write_uint32(uint32_t val, uint8_t *index) {
	config_write_uint16(val >> 16, index);
	config_write_uint16(val, index);
}

void config_write_blob(uint8_t *val, uint8_t len, uint8_t *index) {
	for (uint8_t ind = 0; ind < len; ind++) {
		(&framcfg)[(*index)++] = val[ind];
	}
}

void config_read(config_t *config) {
	uint8_t index = 0;

	config_read_blob(config->id, sizeof(config->id), &index);
	config_read_blob(config->key, sizeof(config->key), &index);
	config_read_blob(config->firmware, sizeof(config->firmware), &index);
	config_read_blob(config->hardware, sizeof(config->hardware), &index);

	config->led_red = config_read_bool(&index);
	config->led_yellow = config_read_bool(&index);
	config->led_green = config_read_bool(&index);
	config->led_cyan = config_read_bool(&index);
	config->led_blue = config_read_bool(&index);
	config->led_purple = config_read_bool(&index);

	config->reading_enable = config_read_bool(&index);
	config->reading_interval = config_read_uint8(&index);
	config->metric_enable = config_read_bool(&index);
	config->metric_interval = config_read_uint8(&index);
	config->buffer_enable = config_read_bool(&index);
	config->buffer_interval = config_read_uint8(&index);

	config->frequency = config_read_uint32(&index);
	config->bandwidth = config_read_uint32(&index);
	config->coding_rate = config_read_uint8(&index);
	config->spreading_factor = config_read_uint8(&index);
	config->preamble_length = config_read_uint8(&index);
	config->checksum = config_read_bool(&index);
	config->tx_power = config_read_uint8(&index);
	config->sync_word = config_read_uint8(&index);
}

void config_write(config_t *config) {
	uint8_t index = 0;

	config_write_blob(config->id, sizeof(config->id), &index);
	config_write_blob(config->key, sizeof(config->key), &index);
	config_write_blob(config->firmware, sizeof(config->firmware), &index);
	config_write_blob(config->hardware, sizeof(config->hardware), &index);

	config_write_bool(config->led_red, &index);
	config_write_bool(config->led_yellow, &index);
	config_write_bool(config->led_green, &index);
	config_write_bool(config->led_cyan, &index);
	config_write_bool(config->led_blue, &index);
	config_write_bool(config->led_purple, &index);

	config_write_bool(config->reading_enable, &index);
	config_write_uint8(config->reading_interval, &index);
	config_write_bool(config->metric_enable, &index);
	config_write_uint8(config->metric_interval, &index);
	config_write_bool(config->buffer_enable, &index);
	config_write_uint8(config->buffer_interval, &index);

	config_write_uint32(config->frequency, &index);
	config_write_uint32(config->bandwidth, &index);
	config_write_uint8(config->coding_rate, &index);
	config_write_uint8(config->spreading_factor, &index);
	config_write_uint8(config->preamble_length, &index);
	config_write_bool(config->checksum, &index);
	config_write_uint8(config->tx_power, &index);
	config_write_uint8(config->sync_word, &index);
}

int config_validate(config_t *config) {
	if (config->reading_interval < 5 || config->reading_interval > 240) {
		return -1;
	}

	if (config->metric_interval < 5 || config->metric_interval > 240) {
		return -1;
	}

	if (config->buffer_interval < 5 || config->buffer_interval > 240) {
		return -1;
	}

	if (config->frequency < 420000000 || config->frequency > 440000000) {
		return -1;
	}

	if (config->bandwidth != 7800 && config->bandwidth != 10400 && config->bandwidth != 15600 && config->bandwidth != 20800 &&
			config->bandwidth != 31250 && config->bandwidth != 41700 && config->bandwidth != 62500 && config->bandwidth != 125000 &&
			config->bandwidth != 250000 && config->bandwidth != 500000) {
		return -1;
	}

	if (config->coding_rate < 5 || config->coding_rate > 8) {
		return -1;
	}

	if (config->spreading_factor < 7 || config->spreading_factor > 12) {
		return -1;
	}

	if (config->preamble_length < 6 || config->preamble_length > 21) {
		return -1;
	}

	if (config->tx_power < 2 || config->tx_power > 17) {
		return -1;
	}

	if (config->sync_word == 0x00 || config->sync_word == 0xff) {
		return -1;
	}

	return 0;
}
