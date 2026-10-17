#include <stdint.h>

#include "main.h"
#include "pin.h"

static void display_mode(uint8_t mode) {
	// Fetch 0b00000xxx and merge to PORTB
	LED_PORT = (0b11111000 & LED_PORT) | (0b00000111 & mode);
	// Fetch 0b0000x000, shift, and merge to PORTB
	LED_PORT = (0b11101111 & LED_PORT) | ((1 << 3 & mode) << 1);
}

void sw1_press_handler(uint8_t* mode) {
	if (*mode < 10)
		++*mode;
	else
		*mode = 0;
	display_mode(*mode);
	// Light up D9
}

void sw1_lift_handler(void* arg) {
	(void)arg;
	// Turn off D9
}

void sw2_press_handler(uint8_t* mode) {
	if (*mode > 0)
		--*mode;
	else
		*mode = 10;
	display_mode(*mode);
	// Light up D10
}

void sw2_lift_handler(void* arg) {
	(void)arg;
	// Turn off D10
}

void sw3_press_handler(void* arg) {
	(void)arg;
	// Light up D11
}

void sw3_lift_handler(void* arg) {
	(void)arg;
	// Turn off D11
}
