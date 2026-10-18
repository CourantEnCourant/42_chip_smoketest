#pragma once

#include <stdbool.h>
#include <stdint.h>

bool     is_all_digit(const char str[], uint8_t size);
bool     is_digit(char c);
uint16_t ft_atoi(const char str[]);
bool     is_valid_date(uint8_t day, uint8_t month, uint16_t year);
int      ft_strlen(const char str[]);

