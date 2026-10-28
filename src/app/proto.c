#include "buffer.h"
#include "core.h"

void encode(entry_t *entry, uplink_t *uplink) {
	uplink->kind = 0x00;
	uplink->data_len = 0;

	if (entry->flags & 0x02) {
		uplink->kind |= 0x02;

		uplink->data[uplink->data_len] = ((entry->photovoltaic >> 4) & 0xff);
		uplink->data_len += sizeof(uplink->data[uplink->data_len]);
		uplink->data[uplink->data_len] = ((entry->photovoltaic & 0x0f) << 4) | ((entry->battery >> 8) & 0x0f);
		uplink->data_len += sizeof(uplink->data[uplink->data_len]);
		uplink->data[uplink->data_len] = ((entry->battery >> 0) & 0xff);
		uplink->data_len += sizeof(uplink->data[uplink->data_len]);
	}
}
