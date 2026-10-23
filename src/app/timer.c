#include "timer.h"
#include "../lib/msp430fr5994.h"
#include <stdint.h>

#define tassel_1 0x0100
#define mc_1 0x0010
#define mc_mask 0x0030
#define taclr 0x0004

#define ccie 0x0010

void timer_init(void) {
	ta0ctl = tassel_1 | taclr;
	ta0cctl0 = ccie;
}

void timer_sleep_s(uint8_t seconds) {
	if (seconds == 0) {
		return;
	}

	ta0ccr0 = 32768 - 1;
	ta0ccr1 = seconds;

	ta0r = 0;
	ta0ctl |= mc_1;

	__asm__ volatile("bis.w #0x00d0, r2");
	__asm__ volatile("nop");

	ta0ctl &= ~mc_mask;
}

void timer_sleep_ms(uint16_t milliseconds) {
	if (milliseconds == 0 || milliseconds > 2000) {
		return;
	}

	uint16_t ticks = (milliseconds << 5) + (milliseconds >> 1) + (milliseconds >> 2) + (milliseconds >> 6) + (milliseconds >> 9);

	ta0ccr0 = ticks - 1;
	ta0ccr1 = 1;

	ta0r = 0;
	ta0ctl |= mc_1;

	__asm__ volatile("bis.w #0x00d0, r2");
	__asm__ volatile("nop");

	ta0ctl &= ~mc_mask;
}
