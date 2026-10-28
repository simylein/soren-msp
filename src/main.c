#include "app/adc49.h"
#include "app/buffer.h"
#include "app/config.h"
#include "app/core.h"
#include "app/led.h"
#include "app/math.h"
#include "app/msp430.h"
#include "app/rtc.h"
#include "app/sx1278.h"
#include "app/timer.h"
#include <stdbool.h>
#include <stdint.h>

int main(void) {
	msp430_clock_8mhz();

	rtc_init();
	timer_init();
	led_init();
	adc49_init();
	sx1278_init();
	buffer_init();

	msp430_gpio_unlock();
	msp430_interrupts_enable();

	if (rtc_seconds() == 0) {
		led_green_blink(200, 100);
	} else {
		led_red_blink(200, 100);
	}

	if (sx1278_sleep() == 0) {
		led_green_blink(200, 100);
	} else {
		led_red_blink(200, 100);
	}

	if (sx1278_version() == 0x12) {
		led_green_blink(200, 100);
	} else {
		led_red_blink(200, 100);
	}

	config_t config;
	config_read(&config);
	config.frame = 0;

	if (sx1278_lora() == -1) {
		led_red_blink(200, 100);
	}
	if (sx1278_standby() == -1) {
		led_red_blink(200, 100);
	}
	if (sx1278_fifo() == -1) {
		led_red_blink(200, 100);
	}
	if (configure(&config) == -1) {
		led_red_blink(200, 100);
	}
	if (sx1278_sleep() == -1) {
		led_red_blink(200, 100);
	}

	while (true) {
		uint32_split_t timestamp = {.value = rtc_seconds()};

		uint16_t photovoltaic = adc49_photovoltaic();
		uint16_t battery = adc49_battery();

		uplink_t uplink;
		uplink.kind = 0x12;

		uplink.data[0] = ((photovoltaic >> 4) & 0xff);
		uplink.data[1] = ((photovoltaic & 0x0f) << 4) | ((battery >> 8) & 0x0f);
		uplink.data[2] = ((battery >> 0) & 0xff);

		uplink.data_len = 3;

		if (sx1278_standby() == -1) {
			led_red_blink(200, 100);
		}

		if (transceive(&config, &uplink) == -1) {
			entry_t entry = {
					.flags = 0x01,
					.photovoltaic = photovoltaic,
					.battery = battery,
					.timestamp = timestamp,
			};
			buffer_push(&entry);
			led_red_blink(200, 100);
			goto sleep;
		}

		timer_sleep_ms(20);
		if (buffer.size > 0) {
			entry_t entry;
			if (buffer_peek(&entry) == -1) {
				goto sleep;
			}

			uint32_split_t diff = {.value = timestamp.value - entry.timestamp.value};
			if (diff.value > 16777215) {
				buffer_pull();
				goto sleep;
			}

			uplink.kind = 0x92;

			uplink.data[0] = ((entry.photovoltaic >> 4) & 0xff);
			uplink.data[1] = ((entry.photovoltaic & 0x0f) << 4) | ((entry.battery >> 8) & 0x0f);
			uplink.data[2] = ((entry.battery >> 0) & 0xff);
			uplink.data[3] = diff.bytes[2];
			uplink.data[4] = diff.bytes[1];
			uplink.data[5] = diff.bytes[0];
			uplink.data[6] = (buffer.size >> 8) & 0xff;
			uplink.data[7] = (buffer.size >> 0) & 0xff;

			uplink.data_len = 8;

			if (transceive(&config, &uplink) == -1) {
				led_red_blink(200, 100);
				goto sleep;
			}

			buffer_pull();
		}

	sleep:
		if (sx1278_sleep() == -1) {
			led_red_blink(200, 100);
		}
		timer_sleep_s(30);
	}
}
