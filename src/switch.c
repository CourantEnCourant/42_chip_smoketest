#include <stdint.h>

#include "led.h"
#include "main.h"

void sw1_press_handler(uint8_t* mode) {
	if (*mode < 10)
		++*mode;
	else
		*mode = 0;
	led_display_mode(*mode);
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
	led_display_mode(*mode);
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
