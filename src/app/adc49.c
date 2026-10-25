#include "../lib/msp430fr5994.h"
#include "timer.h"
#include <stdint.h>

#define adc_pv 0x10
#define adc_pv_en 0x04

#define adc_bat 0x20
#define adc_bat_en 0x08

void adc49_init(void) {
	p4out &= ~(adc_pv_en | adc_bat_en);

	p4dir |= (adc_pv_en | adc_bat_en);

	p1sel0 |= (adc_pv | adc_bat);
	p1sel1 |= (adc_pv | adc_bat);
}

uint16_t adc49_photovoltaic(void) {
	p4out |= adc_pv_en;
	timer_sleep_ms(100);

	adc12ctl0 = 0x0c10;
	adc12ctl1 = 0x0200;
	adc12ctl2 = 0x0020;
	adc12mctl0 = 0x0004;

	uint16_t sum = 0;
	for (uint8_t i = 0; i < 16; i++) {
		adc12ctl0 |= 0x0003;

		while (!(adc12ifgr0 & 0x0001)) {
			__asm__ volatile("nop");
		}

		sum += adc12mem0;
	}

	adc12ctl0 &= ~0x0002;
	adc12ctl0 &= ~0x0010;

	p4out &= ~adc_pv_en;

	return (sum >> 4) + ((sum & 0x000f) >= 8);
}

uint16_t adc49_battery(void) {
	p4out |= adc_bat_en;
	timer_sleep_ms(100);

	adc12ctl0 = 0x0c10;
	adc12ctl1 = 0x0200;
	adc12ctl2 = 0x0020;
	adc12mctl0 = 0x0005;

	uint16_t sum = 0;
	for (uint8_t i = 0; i < 16; i++) {
		adc12ctl0 |= 0x0003;

		while (!(adc12ifgr0 & 0x0001)) {
			__asm__ volatile("nop");
		}

		sum += adc12mem0;
	}

	adc12ctl0 &= ~0x0002;
	adc12ctl0 &= ~0x0010;

	p4out &= ~adc_bat_en;

	return (sum >> 4) + ((sum & 0x000f) >= 8);
}
