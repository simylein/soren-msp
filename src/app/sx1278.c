#include "../lib/msp430fr5994.h"
#include "timer.h"
#include <stdint.h>

#define spi_nss 0x08
#define spi_sck 0x04
#define spi_mosi 0x02
#define spi_miso 0x01

#define sx1278_reg_op_mode 0x01
#define sx1278_reg_version 0x42

uint8_t spi_transfer(uint8_t data) {
	uint8_t received = 0x00;

	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80) {
			p6out |= spi_mosi;
		} else {
			p6out &= ~spi_mosi;
		}

		__asm__ volatile("nop");

		p6out |= spi_sck;
		__asm__ volatile("nop");

		received <<= 1;
		if (p6in & spi_miso) {
			received |= 1;
		}

		p6out &= ~spi_sck;
		__asm__ volatile("nop");

		data <<= 1;
	}

	return received;
}

void sx1278_write(uint8_t reg, uint8_t value) {
	p6out &= ~spi_nss;
	__asm__ volatile("nop");

	spi_transfer(reg | 0x80);
	spi_transfer(value);

	__asm__ volatile("nop");
	p6out |= spi_nss;
}

uint8_t sx1278_read(uint8_t reg) {
	p6out &= ~spi_nss;
	__asm__ volatile("nop");

	spi_transfer(reg & 0x7f);
	uint8_t value = spi_transfer(0x00);

	__asm__ volatile("nop");
	p6out |= spi_nss;

	return value;
}

void sx1278_init(void) {
	p6out |= spi_nss;
	p6out &= ~(spi_sck | spi_mosi);

	p6dir |= spi_nss | spi_sck | spi_mosi;
	p6dir &= ~spi_miso;

	p6sel0 &= ~(spi_nss | spi_sck | spi_mosi | spi_miso);
	p6sel1 &= ~(spi_nss | spi_sck | spi_mosi | spi_miso);
}

uint8_t sx1278_version(void) { return sx1278_read(sx1278_reg_version); }

int sx1278_lora(void) {
	uint8_t op_mode = sx1278_read(sx1278_reg_op_mode);

	op_mode = (op_mode & ~0x88) | 0x88;
	sx1278_write(sx1278_reg_op_mode, op_mode);

	for (uint8_t tries = 0; tries < 5; tries++) {
		uint8_t op_mode = sx1278_read(sx1278_reg_op_mode);

		if ((op_mode & 0x88) == 0x88) {
			return 0;
		}

		timer_sleep_ms(1);
	}

	return -1;
}

int sx1278_sleep(void) {
	uint8_t op_mode = sx1278_read(sx1278_reg_op_mode);

	op_mode = (op_mode & ~0x07) | 0x00;
	sx1278_write(sx1278_reg_op_mode, op_mode);

	for (uint8_t tries = 0; tries < 5; tries++) {
		uint8_t op_mode = sx1278_read(sx1278_reg_op_mode);

		if ((op_mode & 0x07) == 0x00) {
			return 0;
		}

		timer_sleep_ms(1);
	}

	return -1;
}

int sx1278_standby(void) {
	uint8_t op_mode = sx1278_read(sx1278_reg_op_mode);

	op_mode = (op_mode & ~0x07) | 0x01;
	sx1278_write(sx1278_reg_op_mode, op_mode);

	for (uint8_t tries = 0; tries < 5; tries++) {
		uint8_t op_mode = sx1278_read(sx1278_reg_op_mode);

		if ((op_mode & 0x07) == 0x01) {
			return 0;
		}

		timer_sleep_ms(1);
	}

	return -1;
}
