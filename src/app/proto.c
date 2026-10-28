#include "buffer.h"
#include "core.h"
#include <stdint.h>

uint8_t encode(entry_t *entries, uint8_t entries_len, uplink_t *uplink) {
	uint8_t flags = entries[0].flags & 0x0f;

	uplink->kind = 0x00;
	uplink->data_len = 0;

	uint8_t encoded = 0;
	for (uint8_t index = 0; index < entries_len; index++) {
		uint8_t data_len = uplink->data_len;

		if (entries[index].flags != flags) {
			goto cleanup;
		}

		if (flags & 0x01) {
			uplink->data[data_len++] = (entries[index].temperature >> 8) & 0xff;
			uplink->data[data_len++] = (entries[index].temperature >> 0) & 0xff;
			uplink->data[data_len++] = (entries[index].humidity >> 8) & 0xff;
			uplink->data[data_len++] = (entries[index].humidity >> 0) & 0xff;
		}

		if (flags & 0x02) {
			uplink->data[data_len++] = (entries[index].photovoltaic >> 4) & 0xff;
			uplink->data[data_len++] = ((entries[index].photovoltaic & 0x0f) << 4) | ((entries[index].battery >> 8) & 0x0f);
			uplink->data[data_len++] = (entries[index].battery >> 0) & 0xff;
		}

		uplink->data_len = data_len;
		encoded += 1;

		if (index + 1 < entries_len) {
			if (entries[index + 1].flags != flags) {
				goto cleanup;
			}

			uint32_t current = entries[index].timestamp.value;
			uint32_t next = entries[index + 1].timestamp.value;

			if (next < current || next - current > 255) {
				goto cleanup;
			}

			uplink->data[data_len++] = (uint8_t)(next - current);
			uplink->data_len = data_len;
		}
	}

cleanup:
	uplink->kind = (encoded << 4) & 0xf0 | flags;
	return encoded;
}
