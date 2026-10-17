#pragma once

#include <stdint.h>

#define T_OS (314 - 25)
#define K    ((380 - 242) / (85 + 45))

void     adc_init();
uint16_t get_adc();
