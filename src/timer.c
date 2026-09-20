#include "timer.h"
#include <stdint.h>

#define ta0ctl (*(volatile unsigned short *)0x0340)
#define ta0cctl0 (*(volatile unsigned short *)0x0342)
#define ta0r (*(volatile unsigned short *)0x0350)
#define ta0ccr0 (*(volatile unsigned short *)0x0352)
#define ta0ccr1 (*(volatile unsigned short *)0x0354)

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
