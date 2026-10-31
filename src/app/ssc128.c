#include "math.h"
#include <stdint.h>

void diffuse(uint32_split_t value[2], const uint32_split_t key[4]) {
	const uint32_split_t delta = {.value = 0x9e3779b9};

	uint32_split_t alpha = value[0];
	uint32_split_t bravo = value[1];
	uint32_split_t sum = {.value = 0};

	for (uint8_t index = 0; index < 32; index++) {
		alpha = add32(alpha, xor32(add32(xor32(sl4(bravo), sr5(bravo)), bravo), add32(sum, key[sum.words[0] & 3])));
		sum = add32(sum, delta);
		bravo = add32(bravo, xor32(add32(xor32(sl4(alpha), sr5(alpha)), alpha), add32(sum, key[(sum.words[0] >> 11) & 3])));
	}

	value[0] = alpha;
	value[1] = bravo;
}

void ssc128_block(const uint8_t (*key)[16], uint8_t (*input)[16], uint8_t (*output)[16]) {
	uint32_split_t alpha[4];
	uint32_split_t bravo[4];

	for (uint8_t index = 0; index < sizeof(alpha) / sizeof(*alpha); index++) {
		uint8_t offset = index * sizeof(*alpha);

		alpha[index].bytes[0] = (*key)[offset + 3];
		alpha[index].bytes[1] = (*key)[offset + 2];
		alpha[index].bytes[2] = (*key)[offset + 1];
		alpha[index].bytes[3] = (*key)[offset + 0];

		bravo[index].bytes[0] = (*input)[offset + 3];
		bravo[index].bytes[1] = (*input)[offset + 2];
		bravo[index].bytes[2] = (*input)[offset + 1];
		bravo[index].bytes[3] = (*input)[offset + 0];
	}

	diffuse(&bravo[0], alpha);
	diffuse(&bravo[2], alpha);

	for (uint8_t index = 0; index < sizeof(bravo) / sizeof(*bravo); index++) {
		uint8_t offset = index * sizeof(*bravo);

		(*output)[offset + 0] = bravo[index].bytes[0];
		(*output)[offset + 1] = bravo[index].bytes[1];
		(*output)[offset + 2] = bravo[index].bytes[2];
		(*output)[offset + 3] = bravo[index].bytes[3];
	}
}

void ssc128_crypt(uint8_t *data, const uint8_t data_len, const uint16_t frame, const uint8_t (*key)[16]) {
	uint8_t data_block[16];
	uint8_t data_stream[16];

	uint8_t data_ind = 0;
	uint8_t block_ind = 0;

	while (data_ind < data_len) {
		for (uint8_t index = 0; index < sizeof(data_block); index++) {
			data_block[index] = 0x00;
		}

		data_block[0] = (uint8_t)(frame >> 8);
		data_block[1] = (uint8_t)(frame & 0xff);

		data_block[2] = 0;
		data_block[3] = 0;
		data_block[4] = 0;
		data_block[5] = block_ind;

		ssc128_block(key, &data_block, &data_stream);

		for (uint8_t ind = 0; ind < sizeof(data_block) && data_ind < data_len; ind++) {
			data[data_ind++] ^= data_stream[ind];
		}

		block_ind++;
	}
}

void ssc128_encrypt(void *data, const uint8_t data_len, const uint16_t frame, const uint8_t (*key)[16]) {
	ssc128_crypt(data, data_len, frame, key);
}

void ssc128_decrypt(void *data, const uint8_t data_len, const uint16_t frame, const uint8_t (*key)[16]) {
	ssc128_crypt(data, data_len, frame, key);
}
