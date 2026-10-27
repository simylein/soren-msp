#include "../lib/msp430fr5994.h"
#include "math.h"
#include <stdbool.h>
#include <stdint.h>

#define rtc_hold 0x40
#define rtc_source_prescaler 0x08
#define rtc_counter_32bit 0x03

void rtc_init(void) {
	rtcctl0 = 0xa500;
	rtcctl1 = rtc_hold | rtc_source_prescaler | rtc_counter_32bit;

	rtcps0ctl = 0x3800;
	rtcps1ctl = 0xb000;

	rtccnt1 = 0x00;
	rtccnt2 = 0x00;
	rtccnt3 = 0x00;
	rtccnt4 = 0x00;

	rtcctl1 &= ~rtc_hold;
}

uint32_t rtc_seconds(void) {
	uint32_split_t alpha;
	uint32_split_t bravo;

	while (true) {
		alpha.bytes[0] = rtccnt1;
		alpha.bytes[1] = rtccnt2;
		alpha.bytes[2] = rtccnt3;
		alpha.bytes[3] = rtccnt4;

		bravo.bytes[0] = rtccnt1;
		bravo.bytes[1] = rtccnt2;
		bravo.bytes[2] = rtccnt3;
		bravo.bytes[3] = rtccnt4;

		if (alpha.value == bravo.value) {
			break;
		}
	}

	return alpha.value;
}
