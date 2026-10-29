#include "math.h"
#include "config.h"
#include <stdint.h>

uint16_t min16(uint16_t alpha, uint16_t bravo) {
	if (alpha < bravo) {
		return alpha;
	}
	return bravo;
}

uint16_t sub16(uint16_t alpha, uint16_t bravo) {
	if (alpha < bravo) {
		return 0;
	}
	return alpha - bravo;
}

const uint8_t bit_masks[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

uint32_t divide_32(uint32_t numerator, uint32_t denominator) {
	uint32_split_t input = {.value = numerator};
	uint32_split_t result = {.value = 0};

	uint32_t remainder = 0;

	for (int8_t i = 31; i >= 0; i--) {
		uint8_t byte_index = (uint8_t)i >> 3;
		uint8_t bit_index = (uint8_t)i & 7;

		uint8_t mask = bit_masks[bit_index];
		uint8_t bit = (input.bytes[byte_index] & mask) ? 1 : 0;

		remainder = (remainder << 1) | bit;

		if (remainder >= denominator) {
			remainder -= denominator;
			result.bytes[byte_index] |= mask;
		}
	}

	return result.value;
}

uint32_t divide_15625(uint32_t value, uint16_t *rem) {
	uint32_split_t input;
	uint32_split_t result;

	input.value = value;
	result.value = 0;

	uint16_t remainder = 0;

	for (int8_t i = 31; i >= 0; i--) {
		uint8_t byte_index = (uint8_t)i >> 3;
		uint8_t bit_index = (uint8_t)i & 7;

		uint8_t mask = bit_masks[bit_index];
		uint8_t bit = (input.bytes[byte_index] & mask) ? 1 : 0;

		remainder = (uint16_t)((remainder << 1) | bit);

		if (remainder >= 15625u) {
			remainder -= 15625u;
			result.bytes[byte_index] |= mask;
		}
	}

	*rem = remainder;
	return result.value;
}

uint32_t calculate_frf(uint32_t frequency) {
	uint32_split_t quotient;
	uint32_split_t frf;

	uint16_t remainder;

	quotient.value = divide_15625(frequency, &remainder);

	frf.bytes[0] = 0;
	frf.bytes[1] = quotient.bytes[0];
	frf.bytes[2] = quotient.bytes[1];
	frf.bytes[3] = quotient.bytes[2];

	for (int8_t i = 7; i >= 0; i--) {
		remainder = (uint16_t)(remainder << 1);

		if (remainder >= 15625u) {
			remainder -= 15625u;
			frf.bytes[0] |= bit_masks[i];
		}
	}

	return frf.value;
}

const uint16_t symbols[6] = {128, 256, 512, 1024, 2048, 4096};

uint16_t calculate_airtime(config_t *config, uint8_t payload_len) {
	uint16_t symbol_duration = symbols[config->spreading_factor - 7];

	uint32_t preamble_duration = 0;
	for (uint8_t i = 0; i < 4 * config->preamble_length + 17; i++) {
		preamble_duration += symbol_duration;
	}

	int16_t numerator = 8 * payload_len - 4 * config->spreading_factor + 28 + 16 * config->checksum;
	uint16_t denominator = 4 * (config->spreading_factor - 2 * (config->spreading_factor > 10 ? 1 : 0));

	uint16_t payload_symbols = 8;
	if (numerator > 0) {
		uint16_t groups = 0;
		while (numerator > 0) {
			numerator -= denominator;
			groups++;
		}
		for (uint16_t i = 0; i < groups; i++) {
			payload_symbols += config->coding_rate;
		}
	}
	uint32_t payload_duration = 0;
	for (uint16_t i = 0; i < 4 * payload_symbols; i++) {
		payload_duration += symbol_duration;
	}

	uint16_t bandwidth;
	switch (config->bandwidth) {
	case 7800:
		bandwidth = 312;
		break;
	case 10400:
		bandwidth = 416;
		break;
	case 15600:
		bandwidth = 624;
		break;
	case 20800:
		bandwidth = 832;
		break;
	case 31250:
		bandwidth = 1250;
		break;
	case 41700:
		bandwidth = 1668;
		break;
	case 62500:
		bandwidth = 2500;
		break;
	case 125000:
		bandwidth = 5000;
		break;
	case 250000:
		bandwidth = 10000;
		break;
	case 500000:
		bandwidth = 20000;
		break;
	default:
		return 0;
	}

	uint32_t total_airtime = preamble_duration + payload_duration;
	uint32_t milliseconds = divide_32(total_airtime * 10ul - 1, bandwidth) + 1;

	return milliseconds > 65535 ? 65535 : (uint16_t)milliseconds;
}
