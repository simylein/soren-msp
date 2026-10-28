#pragma once

#include "math.h"
#include <stdint.h>

typedef struct entry_t {
	uint8_t flags;
	uint16_t temperature;
	uint16_t humidity;
	uint16_t photovoltaic;
	uint16_t battery;
	uint32_split_t timestamp;
} entry_t;

typedef struct buffer_t {
	uint16_t head;
	uint16_t tail;
	uint16_t size;
} buffer_t;

extern buffer_t buffer;

void buffer_init(void);

int buffer_push(entry_t *entry);
int buffer_peek(uint8_t offset, entry_t *entry);
int buffer_pull(void);
