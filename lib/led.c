#include "led.h"

#include <stdint.h>

#include "pin.h"

void led_setup() {
	set_bit(LED_DDR, D1);
	set_bit(LED_DDR, D2);
	set_bit(LED_DDR, D3);
	set_bit(LED_DDR, D4);
}

void led_all_lights_on() {
	set_bit(LED_PORT, D1);
	set_bit(LED_PORT, D2);
	set_bit(LED_PORT, D3);
	set_bit(LED_PORT, D4);
}

void led_all_lights_off() {
	clear_bit(LED_PORT, D1);
	clear_bit(LED_PORT, D2);
	clear_bit(LED_PORT, D3);
	clear_bit(LED_PORT, D4);
}

void led_display_mode(uint8_t mode) {
	// Fetch 0b00000xxx and merge to PORTB
	LED_PORT = (0b11111000 & LED_PORT) | (0b00000111 & mode);
	// Fetch 0b0000x000, shift, and merge to PORTB
	LED_PORT = (0b11101111 & LED_PORT) | ((1 << 3 & mode) << 1);
}
