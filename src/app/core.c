#include "core.h"
#include "config.h"
#include "sx1278.h"
#include <stdint.h>
#include <string.h>

int configure(config_t *config) {
	if (sx1278_frequency(config->frequency) == -1) {
		return -1;
	}
	if (sx1278_bandwidth(config->bandwidth) == -1) {
		return -1;
	}
	if (sx1278_coding_rate(config->coding_rate) == -1) {
		return -1;
	}
	if (sx1278_spreading_factor(config->spreading_factor) == -1) {
		return -1;
	}
	if (sx1278_preamble_length(config->preamble_length) == -1) {
		return -1;
	}
	if (sx1278_checksum(config->checksum) == -1) {
		return -1;
	}
	if (sx1278_tx_power(config->tx_power) == -1) {
		return -1;
	}
	if (sx1278_sync_word(config->sync_word) == -1) {
		return -1;
	}

	return 0;
}

int transceive(config_t *config, uplink_t *uplink) {
	uint8_t tx_data[256];
	uint8_t tx_data_len = 0;

	tx_data[tx_data_len] = config->id[0];
	tx_data_len += sizeof(config->id[0]);
	tx_data[tx_data_len] = config->id[1];
	tx_data_len += sizeof(config->id[1]);
	tx_data[tx_data_len] = (config->frame >> 8) & 0xff;
	tx_data_len += sizeof(tx_data[tx_data_len]);
	tx_data[tx_data_len] = (config->frame >> 0) & 0xff;
	tx_data_len += sizeof(tx_data[tx_data_len]);
	tx_data[tx_data_len] = ((config->tx_power - 2) << 4) & 0xf0 | ((config->preamble_length - 6) & 0x0f);
	tx_data_len += sizeof(tx_data[tx_data_len]);
	tx_data[tx_data_len] = uplink->kind;
	tx_data_len += sizeof(tx_data[tx_data_len]);

	for (uint8_t index = 0; index < uplink->data_len; index++) {
		tx_data[tx_data_len] = uplink->data[index];
		tx_data_len += sizeof(tx_data[tx_data_len]);
	}

	if (sx1278_transmit(tx_data, tx_data_len, 512) == -1) {
		return -1;
	}

	config->frame += 1;

	uint8_t rx_data[256];
	uint8_t rx_data_len = 0;
	bool crc_used = false;
	bool crc_fail = false;

	if (sx1278_receive(rx_data, &rx_data_len, 512, &crc_used, &crc_fail) == -1) {
		return -1;
	}

	if (crc_used == true && crc_fail == true) {
		return -1;
	}

	if (rx_data_len < 6) {
		return -1;
	}

	if (rx_data[0] != config->id[0] || rx_data[1] != config->id[1]) {
		return -1;
	}

	return 0;
}
