#pragma once

#include <stdbool.h>
#include <stdint.h>

void sx1278_init(void);

uint8_t sx1278_version(void);

int sx1278_lora(void);
int sx1278_sleep(void);
int sx1278_standby(void);

int sx1278_fifo(void);

int sx1278_frequency(uint32_t frequency);
int sx1278_tx_power(uint8_t power);
int sx1278_preamble_length(uint16_t len);
int sx1278_bandwidth(uint32_t bandwidth);
int sx1278_coding_rate(uint8_t cr);
int sx1278_spreading_factor(uint8_t sf);
int sx1278_checksum(bool crc);
int sx1278_sync_word(uint8_t word);

int sx1278_transmit(uint8_t *data, uint8_t length, uint16_t timeout);
