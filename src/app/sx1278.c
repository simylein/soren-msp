#include "../lib/msp430fr5994.h"
#include "math.h"
#include "timer.h"
#include <stdbool.h>
#include <stdint.h>

#define spi_nss 0x08
#define spi_sck 0x04
#define spi_mosi 0x02
#define spi_miso 0x01

#define sx1278_reg_op_mode 0x01
#define sx1278_reg_frf_msb 0x06
#define sx1278_reg_frf_mid 0x07
#define sx1278_reg_frf_lsb 0x08
#define sx1278_reg_pa_config 0x09
#define sx1278_reg_modem_config_1 0x1d
#define sx1278_reg_modem_config_2 0x1e
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

	op_mode = (op_mode & ~0x8f) | 0x88;
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

int sx1278_frequency(uint32_t frequency) {
	uint32_split_t frf;
	frf.value = calculate_frf(frequency);

	sx1278_write(sx1278_reg_frf_msb, frf.bytes[2]);
	sx1278_write(sx1278_reg_frf_mid, frf.bytes[1]);
	sx1278_write(sx1278_reg_frf_lsb, frf.bytes[0]);

	if (sx1278_read(sx1278_reg_frf_msb) != frf.bytes[2]) {
		return -1;
	}
	if (sx1278_read(sx1278_reg_frf_mid) != frf.bytes[1]) {
		return -1;
	}
	if (sx1278_read(sx1278_reg_frf_lsb) != frf.bytes[0]) {
		return -1;
	}

	return 0;
}

int sx1278_tx_power(uint8_t power) {
	if (power < 2) {
		power = 2;
	}
	if (power > 17) {
		power = 17;
	}

	uint8_t pa_config = 0x80 | (power - 2);
	sx1278_write(sx1278_reg_pa_config, pa_config);

	if (sx1278_read(sx1278_reg_pa_config) != pa_config) {
		return -1;
	}

	return 0;
}

int sx1278_bandwidth(uint32_t bandwidth) {
	uint8_t bw_bits;

	switch (bandwidth) {
	case 7800:
		bw_bits = 0x0;
		break;
	case 10400:
		bw_bits = 0x1;
		break;
	case 15600:
		bw_bits = 0x2;
		break;
	case 20800:
		bw_bits = 0x3;
		break;
	case 31250:
		bw_bits = 0x4;
		break;
	case 41700:
		bw_bits = 0x5;
		break;
	case 62500:
		bw_bits = 0x6;
		break;
	case 125000:
		bw_bits = 0x7;
		break;
	case 250000:
		bw_bits = 0x8;
		break;
	case 500000:
		bw_bits = 0x9;
		break;
	default:
		return -1;
	}

	uint8_t modem_config_1 = sx1278_read(sx1278_reg_modem_config_1);
	modem_config_1 = (modem_config_1 & 0x0f) | (bw_bits << 4);

	sx1278_write(sx1278_reg_modem_config_1, modem_config_1);

	if (sx1278_read(sx1278_reg_modem_config_1) != modem_config_1) {
		return -1;
	}

	return 0;
}

int sx1278_coding_rate(uint8_t cr) {
	if (cr < 5) {
		cr = 5;
	}
	if (cr > 8) {
		cr = 8;
	}

	uint8_t modem_config_1 = sx1278_read(sx1278_reg_modem_config_1);
	modem_config_1 = (modem_config_1 & 0xf1) | ((cr - 4) << 1);

	sx1278_write(sx1278_reg_modem_config_1, modem_config_1);

	if (sx1278_read(sx1278_reg_modem_config_1) != modem_config_1) {
		return -1;
	}

	return 0;
}

int sx1278_spreading_factor(uint8_t sf) {
	if (sf < 7) {
		sf = 7;
	}
	if (sf > 12) {
		sf = 12;
	}

	uint8_t modem_config_2 = sx1278_read(sx1278_reg_modem_config_2);
	modem_config_2 = (modem_config_2 & 0x0f) | (sf << 4);

	sx1278_write(sx1278_reg_modem_config_2, modem_config_2);

	if (sx1278_read(sx1278_reg_modem_config_2) != modem_config_2) {
		return -1;
	}

	return 0;
}

int sx1278_checksum(bool crc) {
	uint8_t modem_config_2 = sx1278_read(sx1278_reg_modem_config_2);
	modem_config_2 = (modem_config_2 & ~0x04) | ((crc & 1) << 2);

	sx1278_write(sx1278_reg_modem_config_2, modem_config_2);

	if (sx1278_read(sx1278_reg_modem_config_2) != modem_config_2) {
		return -1;
	}

	return 0;
}
