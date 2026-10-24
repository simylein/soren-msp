#include "../lib/msp430fr5994.h"

#define locklpm5 0x01

void msp430_interrupts_enable(void) {
	__asm__ volatile("nop");
	__asm__ volatile("eint");
	__asm__ volatile("nop");
}

void msp430_gpio_unlock(void) { pm5ctl0 &= ~locklpm5; }
