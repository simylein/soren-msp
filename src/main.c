#include "app/adc49.h"
#include "app/buffer.h"
#include "app/config.h"
#include "app/core.h"
#include "app/led.h"
#include "app/math.h"
#include "app/msp430.h"
#include "app/proto.h"
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

	config_t config;
	config_read(&config);
	config.frame = 0;
	if (config_validate(&config) == 0) {
		led_green_blink(200, 100);
	} else if (config.id[0] == 0xff && config.id[1] == 0xff) {
		config.reading_enable = true;
		config.reading_interval = 60;
		config.metric_enable = true;
		config.metric_interval = 60;
		config.buffer_enable = true;
		config.buffer_interval = 30;
		config.live_batch = 1;
		config.buffer_batch = 1;
		config.frequency = 433225000;
		config.bandwidth = 125000;
		config.coding_rate = 5;
		config.spreading_factor = 7;
		config.preamble_length = 6;
		config.checksum = true;
		config.tx_power = 2;
		config.sync_word = 0x16;
		config_write(&config);
		led_blue_blink(2000, 100);
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

	bool acknowledged = false;

	uint16_t next_reading = 0;
	uint16_t next_metric = 0;
	uint16_t next_buffer = 0;
	uint16_t next_heartbeat = 1200;

	entry_t stash[4];
	uint8_t stash_len = 0;
	while (true) {
		bool do_reading = next_reading == 0 && config.reading_enable == true;
		bool do_metric = next_metric == 0 && config.metric_enable == true;
		bool do_buffer = next_buffer == 0 && config.buffer_enable == true;

		uint32_split_t timestamp = {.value = rtc_seconds()};

		if (do_reading || do_metric) {
			entry_t entry = {.flags = 0x00, .timestamp = timestamp};

			if (do_reading == true) {
				entry.flags |= 0x01;
			}

			if (do_metric == true) {
				entry.flags |= 0x02;
				entry.photovoltaic = adc49_photovoltaic();
				entry.battery = adc49_battery();
			}

			stash[stash_len++] = entry;
		}

		if (stash_len >= config.live_batch) {
			uint8_t offset = 0;

			while (offset < stash_len) {
				uplink_t uplink;
				uint8_t encoded = encode(&stash[offset], stash_len - offset, &uplink);

				uint32_t diff = timestamp.value - stash[offset + encoded - 1].timestamp.value;
				if (diff > 0) {
					for (uint8_t index = offset; index < stash_len; index++) {
						buffer_push(&stash[index]);
					}
					stash_len = 0;
					goto sleep;
				}

				if (sx1278_standby() == -1) {
					led_red_blink(200, 100);
				}

				if (transceive(&config, &uplink) == -1) {
					acknowledged = false;
					for (uint8_t index = offset; index < stash_len; index++) {
						buffer_push(&stash[index]);
					}
					stash_len = 0;

					led_red_blink(200, 100);
					goto sleep;
				}

				offset += encoded;
			}

			stash_len = 0;
			acknowledged = true;
		}

		if (do_buffer == true && acknowledged == true && buffer.size >= config.buffer_batch) {
			entry_t entries[4];
			uint8_t entries_len = 0;

			for (uint8_t index = 0; index < config.buffer_batch && index < buffer.size; index++) {
				if (buffer_peek(index, &entries[index]) == -1) {
					goto sleep;
				}

				entries_len++;
			}

			uplink_t uplink;
			uint8_t encoded = encode(entries, entries_len, &uplink);

			uint32_split_t diff = {.value = timestamp.value - entries[encoded - 1].timestamp.value};
			if (diff.value > 16777215) {
				for (uint8_t index = 0; index < encoded; index++) {
					buffer_pull();
				}
				goto sleep;
			}

			uplink.kind |= 0x80;
			uplink.data[uplink.data_len] = diff.bytes[2];
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = diff.bytes[1];
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = diff.bytes[0];
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = (buffer.size >> 8) & 0xff;
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = (buffer.size >> 0) & 0xff;
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);

			if (sx1278_standby() == -1) {
				led_red_blink(200, 100);
			}

			if (transceive(&config, &uplink) == -1) {
				led_red_blink(200, 100);
				goto sleep;
			}

			for (uint8_t index = 0; index < encoded; index++) {
				buffer_pull();
			}

			if (buffer.size == 0) {
				next_heartbeat = 0;
			}
		}

		if (next_heartbeat == 0) {
			uint32_split_t diff = {.value = 0};

			entry_t entry;
			if (buffer_peek(0, &entry) == 0) {
				diff.value = timestamp.value - entry.timestamp.value;
				if (diff.value > 16777215) {
					diff.value = 16777215;
				}
			}

			uplink_t uplink;
			uplink.kind = 0x80;
			uplink.data_len = 0;
			uplink.data[uplink.data_len] = diff.bytes[2];
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = diff.bytes[1];
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = diff.bytes[0];
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = (buffer.size >> 8) & 0xff;
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);
			uplink.data[uplink.data_len] = (buffer.size >> 0) & 0xff;
			uplink.data_len += sizeof(uplink.data[uplink.data_len]);

			if (sx1278_standby() == -1) {
				led_red_blink(200, 100);
			}

			if (transceive(&config, &uplink) == -1) {
				led_red_blink(200, 100);
				goto sleep;
			}
		}

	sleep:
		if (sx1278_sleep() == -1) {
			led_red_blink(200, 100);
		}

		if (next_reading == 0) {
			next_reading = config.reading_interval;
		}
		if (next_metric == 0) {
			next_metric = config.metric_interval;
		}
		if (next_buffer == 0) {
			next_buffer = config.buffer_interval;
		}
		if (next_heartbeat == 0) {
			next_heartbeat = 1200;
		}

		uint16_t interval = min16(min16(next_reading, next_metric), next_heartbeat);
		if (acknowledged == true && buffer.size > 0) {
			interval = min16(interval, next_buffer);
		}

		timer_sleep_s(interval);

		next_reading = sub16(next_reading, interval);
		next_metric = sub16(next_metric, interval);
		next_buffer = sub16(next_buffer, interval);
		next_heartbeat = sub16(next_heartbeat, interval);
	}
}
