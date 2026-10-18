#include "led.h"

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
