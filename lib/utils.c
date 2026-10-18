#include "utils.h"

#include <stdint.h>

static bool is_leap_year(uint16_t year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static uint8_t days_in_month(uint8_t month, uint16_t year) {
	static const uint8_t days[12] = {31, 28, 31, 30, 31, 30,
	                                 31, 31, 30, 31, 30, 31};
	if (month == 2 && is_leap_year(year))
		return 29;
	return days[month - 1];
}

bool is_valid_date(uint8_t day, uint8_t month, uint16_t year) {
	if (month < 1 || month > 12)
		return false;
	if (day < 1 || day > days_in_month(month, year))
		return false;
	return true;
}

int ft_strlen(const char str[]) {
	int i = 0;
	while (str[i]) {
		i++;
	}
	return i;
}

uint16_t ft_atoi(const char str[]) {
	uint16_t res = 0;
	int      i = 0;
	while ('0' <= str[i] && str[i] <= '9') {
		res = res * 10 + str[i] - '0';
		i++;
	}
	return res;
}

bool is_digit(char c) {
	return ('0' <= c && c <= '9');
}

bool is_all_digit(const char str[], uint8_t size) {
	for (uint8_t i = 0; i < size; i++) {
		if (!is_digit(str[i]))
			return false;
	}
	return true;
}

