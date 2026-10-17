#include "main.h"
#include "pin.h"

void sw1_press_handler(void* arg) {
	(void)arg;
	// Light up D1
	set_bit(LED_PORT, D1);
}

void sw1_lift_handler(void* arg) {
	(void)arg;
	// Turn off D1
	clear_bit(LED_PORT, D1);
}

void sw2_press_handler(void* arg) {
	(void)arg;
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
