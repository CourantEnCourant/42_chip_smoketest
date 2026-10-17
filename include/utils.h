#pragma once

#include <stdint.h>

uint8_t is_all_digit(const char *str, uint8_t size);
uint8_t is_digit(char c);
uint16_t ft_atoi(const char *str);
uint8_t is_valid_date(uint8_t day, uint8_t month, uint16_t year);
int ft_strlen(const char *str);