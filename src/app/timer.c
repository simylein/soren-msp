#include "timer.h"
#include "../lib/msp430fr5994.h"
#include <stdint.h>

#define tassel_1 0x0100
#define mc_1 0x0010
#define taclr 0x0004

#define ccie 0x0010

void timer_init(void) {
	ta0ctl = tassel_1 | taclr;
	ta0ccr0 = 32768 - 1;
	ta0cctl0 = ccie;
}

void timer_sleep_s(uint8_t seconds) {
	if (seconds == 0) {
		return;
	}

	ta0ccr1 = seconds;

	ta0r = 0;
	ta0ctl |= mc_1;

	__asm__ volatile("bis.w #0x00d0, r2");
	__asm__ volatile("nop");
}
