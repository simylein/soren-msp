#include "buffer.h"
#include "../lib/msp430fr5994.h"
#include "math.h"
#include <stdint.h>

#define entry_len 12
#define buffer_len 2048

buffer_t buffer;

void buffer_init(void) {
	buffer.head = 0;
	buffer.tail = 0;
	buffer.size = 0;
}

int buffer_push(entry_t *entry) {
	int status;

	if (buffer.size >= buffer_len) {
		buffer_pull();
		status = -1;
	} else {
		status = 0;
	}

	uint16_t index = buffer.tail * entry_len;

	(&framstrg)[index++] = 0x50 | (entry->flags & 0x0f);
	(&framstrg)[index++] = ((entry->temperature >> 8) & 0xff);
	(&framstrg)[index++] = ((entry->temperature >> 0) & 0xff);
	(&framstrg)[index++] = ((entry->humidity >> 8) & 0xff);
	(&framstrg)[index++] = ((entry->humidity >> 0) & 0xff);
	(&framstrg)[index++] = ((entry->photovoltaic >> 4) & 0xff);
	(&framstrg)[index++] = ((entry->photovoltaic & 0x0f) << 4) | ((entry->battery >> 8) & 0x0f);
	(&framstrg)[index++] = ((entry->battery >> 0) & 0xff);
	(&framstrg)[index++] = entry->timestamp.bytes[3];
	(&framstrg)[index++] = entry->timestamp.bytes[2];
	(&framstrg)[index++] = entry->timestamp.bytes[1];
	(&framstrg)[index++] = entry->timestamp.bytes[0];

	buffer.tail += 1;
	buffer.tail %= buffer_len;
	buffer.size += 1;

	return status;
}

int buffer_peek(entry_t *entry) {
	if (buffer.size == 0) {
		return -1;
	}

	uint16_t index = buffer.head * entry_len;

	if (((&framstrg)[index] & 0xf0) != 0x50) {
		return -1;
	}

	entry->flags = (&framstrg)[index++] & 0x0f;
	entry->temperature = (uint16_t)(&framstrg)[index++] << 8;
	entry->temperature |= (&framstrg)[index++] << 0;
	entry->humidity = (uint16_t)(&framstrg)[index++] << 8;
	entry->humidity |= (&framstrg)[index++] << 0;
	entry->photovoltaic = (&framstrg)[index++] << 4;
	entry->photovoltaic |= ((&framstrg)[index] >> 4) & 0x0f;
	entry->battery = ((&framstrg)[index++] & 0x0f) << 8;
	entry->battery |= (&framstrg)[index++];
	entry->timestamp.bytes[3] = (&framstrg)[index++];
	entry->timestamp.bytes[2] = (&framstrg)[index++];
	entry->timestamp.bytes[1] = (&framstrg)[index++];
	entry->timestamp.bytes[0] = (&framstrg)[index++];

	return 0;
}

int buffer_pull(void) {
	if (buffer.size == 0) {
		return -1;
	}

	buffer.head += 1;
	buffer.head %= buffer_len;
	buffer.size -= 1;

	return 0;
}
