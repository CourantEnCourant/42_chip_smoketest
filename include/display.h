#pragma once

#include <stdbool.h>
#include <stdint.h>

// 4 digit seven segment display behind the PCA9555
bool display_init(void);
void display_refresh(void);
void display_set_digits(const uint8_t digits[4]);
void display_set_number(uint16_t value);
void display_set_BCD(uint8_t val1, uint8_t val2);