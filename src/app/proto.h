#pragma once

#include "buffer.h"
#include "core.h"
#include <stdint.h>

uint8_t encode(entry_t *entries, uint8_t entries_len, uplink_t *uplink);
