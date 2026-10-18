#pragma once

#include "i2c.h"

#define AHT20_ADDR 0x38
#define TW_WRITE 0
#define TW_READ 1

void trigger_measure(void);
void get_mesure(uint32_t *sh, uint32_t *st);