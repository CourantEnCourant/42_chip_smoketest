#include "display.h"

#include <avr/io.h>
#include <stdint.h>

#include "ft_interrupt.h"
#include "pca.h"

static uint8_t       display_buffer[4] = {SEG_NONE, SEG_NONE, SEG_NONE, SEG_NONE};
static const uint8_t slots[4] = {PCA04_CA1, PCA05_CA2, PCA06_CA3, PCA07_CA4};

bool display_init(void) {
	return pca_API_screen_init();
}

// multiplexing: light one digit per call
void display_refresh(void) {
	static uint8_t slot = 0;
	static uint8_t prev = 3;

	// erase old digit
	pca_API_clear_slot(slots[prev]);
	// prepare segments
	pca_API_select_segments(display_buffer[slot]);
	// start cathode
	pca_API_select_cathode(slots[slot]);

	prev = slot;
	slot = (slot + 1) & 0b11;
}

// digits are 0-9, or DIGIT_BLANK for an empty slot
void display_set_digits(const uint8_t digits[4]) {
	uint8_t segs[4];

	for (uint8_t i = 0; i < 4; ++i)
		segs[i] = pca_API_segment(digits[i]);

	uint8_t sreg = SREG;
	ft_cli();
	for (uint8_t i = 0; i < 4; ++i)
		display_buffer[i] = segs[i];
	SREG = sreg;
}

void display_set_number(uint16_t value) {
	uint8_t out[4];

	out[0] = value / 1000 % 10;
	out[1] = value / 100 % 10;
	out[2] = value / 10 % 10;
	out[3] = value % 10;

	// blank leading zeros, keep the last digit
	for (uint8_t i = 0; i < 3 && out[i] == 0; ++i)
		out[i] = DIGIT_BLANK;

	display_set_digits(out);
}

void display_set_BCD(uint8_t val1, uint8_t val2) {
	uint8_t out[4];

	out[0] = val1 >> 4;
	out[1] = val1 & 0x0F;
	out[2] = val2 >> 4;
	out[3] = val2 & 0x0F;

	display_set_digits(out);
}