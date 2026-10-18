#pragma once

#include "i2c.h"

#define AHT20_ADDR 0x38
#define TW_WRITE 0
#define TW_READ 1

void trigger_measure(void);
uint8_t get_mesure(uint32_t *sh, uint32_t *st);
int16_t aht20_temp(uint32_t st);
int16_t aht20_hum(uint32_t sh);