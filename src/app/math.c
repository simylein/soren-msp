#include "math.h"
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
