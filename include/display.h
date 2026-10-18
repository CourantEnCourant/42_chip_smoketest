#ifndef DISPLAY_H
# define DISPLAY_H

# include <stdbool.h>
# include <stdint.h>

# define DISPLAY_BLANK 10

bool display_init(void);

void display_tick(void);

void display_service(void);

void display_set_digits(const uint8_t digits[4]);
void display_set_number(uint16_t value);
void display_clear(void);

bool display_has_error(void);
bool display_bus_free(void);

void display_set_raw_segments(const uint8_t segments[4]);

#endif