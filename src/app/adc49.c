#include "../lib/msp430fr5994.h"
#include "timer.h"
#include <stdint.h>

#define adc_pv 0x10
#define adc_pv_en 0x04
#define adc_pv_chan 0x0004

#define adc_bat 0x20
#define adc_bat_en 0x08
#define adc_bat_chan 0x0005

void adc49_init(void) {
	p4out &= ~(adc_pv_en | adc_bat_en);

	p4dir |= (adc_pv_en | adc_bat_en);

	p1sel0 |= (adc_pv | adc_bat);
	p1sel1 |= (adc_pv | adc_bat);
}

int adc_convert(uint16_t channel, uint16_t *value) {
	int status;

	adc12ctl0 = 0x0c10;
	adc12ctl1 = 0x0200;
	adc12ctl2 = 0x0020;
	adc12mctl0 = channel;

	uint16_t sum = 0;
	for (uint8_t i = 0; i < 16; i++) {
		adc12ctl0 |= 0x0003;

		uint16_t timeout = 16384;
		while (!(adc12ifgr0 & 0x0001) && timeout > 0) {
			__asm__ volatile("nop");
			timeout -= 1;
		}

		if (!(adc12ifgr0 & 0x0001)) {
			status = -1;
			goto cleanup;
		}

		sum += adc12mem0;
	}

	status = 0;
	*value = (sum >> 4) + ((sum & 0x000f) >= 8);

cleanup:
	adc12ctl0 &= ~0x0002;
	adc12ctl0 &= ~0x0010;
	return status;
}

int adc49_read(uint16_t *photovoltaic, uint16_t *battery) {
	int status;

	p4out |= (adc_pv_en | adc_bat_en);
	timer_sleep_ms(100);

	if (adc_convert(adc_pv_chan, photovoltaic) == -1) {
		status = -1;
		goto cleanup;
	}

	if (adc_convert(adc_bat_chan, battery) == -1) {
		status = -1;
		goto cleanup;
	}

	status = 0;

cleanup:
	p4out &= ~(adc_pv_en | adc_bat_en);
	return status;
}
