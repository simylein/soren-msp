#include "math.h"
#include <stdint.h>

uint32_split_t ssc128_get32(const uint8_t *ptr) {
	uint32_split_t val;

	val.bytes[0] = ptr[3];
	val.bytes[1] = ptr[2];
	val.bytes[2] = ptr[1];
	val.bytes[3] = ptr[0];

	return val;
}

void ssc128_put32(uint8_t *ptr, uint32_split_t val) {
	ptr[0] = val.bytes[3];
	ptr[1] = val.bytes[2];
	ptr[2] = val.bytes[1];
	ptr[3] = val.bytes[0];
}

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

void ssc128_block(const uint8_t (*key)[16], const uint8_t (*input)[16], uint8_t (*output)[16]) {
	uint32_split_t alpha[4];
	uint32_split_t bravo[4];

	for (uint8_t i = 0; i < 4; i++) {
		alpha[i] = ssc128_get32(&(*key)[i * 4]);
		bravo[i] = ssc128_get32(&(*input)[i * 4]);
	}

	diffuse(&bravo[0], alpha);
	diffuse(&bravo[2], alpha);

	for (uint8_t i = 0; i < 4; i++) {
		ssc128_put32(&(*output)[i * 4], bravo[i]);
	}
}

void ssc128_mic(const uint8_t *data, const uint8_t data_len, const uint8_t (*id)[2], const uint16_t frame, const uint8_t kind,
								const uint8_t radio, const uint8_t dir, uint8_t (*mic)[2], const uint8_t (*key)[16]) {
	uint32_split_t master[4];

	for (uint8_t i = 0; i < 4; i++) {
		master[i] = ssc128_get32(&(*key)[i * 4]);
	}

	uint32_split_t seed[2][2] = {{{.value = 0x7373636d}, {.value = 0x61633080}}, {{.value = 0x7373636d}, {.value = 0x61633081}}};

	diffuse(seed[0], master);
	diffuse(seed[1], master);

	uint32_split_t mac_key[4] = {seed[0][0], seed[0][1], seed[1][0], seed[1][1]};

	uint8_t header[8] = {(*id)[0], (*id)[1], (uint8_t)(frame >> 8), (uint8_t)frame, kind, radio, dir, data_len};

	uint32_split_t state[2] = {ssc128_get32(header), ssc128_get32(header + 4)};

	diffuse(state, mac_key);

	for (uint16_t offset = 0; offset < data_len; offset += 8) {
		for (uint8_t i = 0; i < 8 && offset + i < data_len; i++) {
			state[i / 4].bytes[3 - (i % 4)] ^= data[offset + i];
		}

		diffuse(state, mac_key);
	}

	(*mic)[0] = state[0].bytes[3];
	(*mic)[1] = state[0].bytes[2];
}

void ssc128_crypt(uint8_t *data, const uint8_t data_len, const uint8_t (*id)[2], const uint16_t frame, const uint8_t kind,
									const uint8_t radio, const uint8_t dir, const uint8_t (*key)[16]) {
	uint8_t data_block[16];
	uint8_t data_stream[16];

	uint8_t data_ind = 0;
	uint8_t block_ind = 0;

	while (data_ind < data_len) {
		for (uint8_t index = 0; index < 2; index++) {
			uint8_t offset = index * 8;

			data_block[offset + 0] = (*id)[0];
			data_block[offset + 1] = (*id)[1];

			data_block[offset + 2] = (uint8_t)(frame >> 8);
			data_block[offset + 3] = (uint8_t)(frame & 0xff);

			data_block[offset + 4] = kind;
			data_block[offset + 5] = radio;

			data_block[offset + 6] = dir;
			data_block[offset + 7] = block_ind;

			block_ind += 1;
		}

		ssc128_block(key, &data_block, &data_stream);

		for (uint8_t index = 0; index < sizeof(data_stream) && data_ind < data_len; index++) {
			data[data_ind++] ^= data_stream[index];
		}
	}
}

void ssc128_encrypt(uint8_t *data, const uint8_t data_len, const uint8_t (*id)[2], const uint16_t frame, const uint8_t kind,
										const uint8_t radio, const uint8_t dir, uint8_t (*mic)[2], const uint8_t (*key)[16]) {
	ssc128_crypt(data, data_len, id, frame, kind, radio, dir, key);
	ssc128_mic(data, data_len, id, frame, kind, radio, dir, mic, key);
}

void ssc128_decrypt(uint8_t *data, const uint8_t data_len, const uint8_t (*id)[2], const uint16_t frame, const uint8_t kind,
										const uint8_t radio, const uint8_t dir, const uint8_t (*mic)[2], const uint8_t (*key)[16], bool *mic_fail) {
	uint8_t mac[2];
	ssc128_mic(data, data_len, id, frame, kind, radio, dir, &mac, key);

	if (mac[0] != (*mic)[0] || mac[1] != (*mic)[1]) {
		*mic_fail = true;
		return;
	}

	ssc128_crypt(data, data_len, id, frame, kind, radio, dir, key);
}
