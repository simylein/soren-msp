#include "../lib/msp430fr5994.h"
#include "timer.h"
#include <stdint.h>

#define i2c_sda 0x01
#define i2c_scl 0x02

#define sht45_address 0x46
#define sht45_measure 0xfd

void i2c_delay(void) {
	for (uint8_t i = 0; i < 16; i++) {
		__asm__ volatile("nop");
	}
}

int scl_high(void) {
	p5dir &= ~i2c_scl;

	for (uint8_t i = 0; i < 32; i++) {
		if (p5in & i2c_scl) {
			i2c_delay();
			return 0;
		}
		i2c_delay();
	}

	return -1;
}

int i2c_start(void) {
	p5dir &= ~i2c_sda;

	if (scl_high() != 0) {
		return -1;
	}
	if (!(p5in & i2c_sda)) {
		return -1;
	}

	p5dir |= i2c_sda;
	i2c_delay();
	p5dir |= i2c_scl;

	return 0;
}

void i2c_stop(void) {
	p5dir |= i2c_sda;
	i2c_delay();
	scl_high();

	p5dir &= ~i2c_sda;
	i2c_delay();
}

int i2c_write(uint8_t data) {
	for (uint8_t bit = 0; bit < 8; bit++) {

		p5dir |= i2c_scl;

		if (data & 0x80) {
			p5dir &= ~i2c_sda;
		} else {
			p5dir |= i2c_sda;
		}

		i2c_delay();

		if (scl_high() != 0) {
			return -1;
		}

		i2c_delay();

		p5dir |= i2c_scl;
		i2c_delay();

		data <<= 1;
	}

	p5dir &= ~i2c_sda;
	i2c_delay();

	if (scl_high() != 0) {
		return -1;
	}

	i2c_delay();
	int ack = (p5in & i2c_sda) ? -1 : 0;

	p5dir |= i2c_scl;
	i2c_delay();

	return ack;
}

int i2c_read(uint8_t *value, uint8_t ack) {
	uint8_t data = 0;
	p5dir &= ~i2c_sda;

	for (uint8_t bit = 0; bit < 8; bit++) {
		data <<= 1;
		i2c_delay();

		if (scl_high() != 0) {
			return -1;
		}
		if (p5in & i2c_sda) {
			data |= 1;
		}

		p5dir |= i2c_scl;
	}

	if (ack) {
		p5dir |= i2c_sda;
	} else {
		p5dir &= ~i2c_sda;
	}

	i2c_delay();
	if (scl_high() != 0) {
		return -1;
	}

	p5dir |= i2c_scl;
	p5dir &= ~i2c_sda;

	*value = data;
	return 0;
}

uint8_t crc8(uint8_t *bytes) {
	uint8_t crc = 0xff;

	for (uint8_t i = 0; i < 2; i++) {
		crc ^= bytes[i];
		for (uint8_t b = 0; b < 8; b++) {
			crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
		}
	}

	return crc;
}

void sht45_init(void) {
	p5out &= ~(i2c_sda | i2c_scl);

	p5dir &= ~(i2c_sda | i2c_scl);

	p5sel0 &= ~(i2c_sda | i2c_scl);
	p5sel1 &= ~(i2c_sda | i2c_scl);

	p5ren &= ~(i2c_sda | i2c_scl);
}

int sht45_read(uint16_t *temperature, uint16_t *humidity) {
	if (i2c_start() != 0) {
		i2c_stop();
		return -1;
	}

	if (i2c_write(sht45_address << 1) != 0) {
		i2c_stop();
		return -1;
	}

	if (i2c_write(sht45_measure) != 0) {
		i2c_stop();
		return -1;
	}

	i2c_stop();
	timer_sleep_ms(10);

	if (i2c_start() != 0) {
		i2c_stop();
		return -1;
	}

	if (i2c_write((sht45_address << 1) | 1) != 0) {
		i2c_stop();
		return -1;
	}

	uint8_t data[6];
	for (uint8_t i = 0; i < 6; i++) {
		if (i2c_read(&data[i], i != 5) != 0) {
			i2c_stop();
			return -1;
		}
	}

	i2c_stop();
	if (crc8(&data[0]) != data[2] || crc8(&data[3]) != data[5]) {
		return -1;
	}

	*temperature = ((uint16_t)data[0] << 8) | data[1] & 0xff;
	*humidity = ((uint16_t)data[3] << 8) | data[4] & 0xff;
	return 0;
}
