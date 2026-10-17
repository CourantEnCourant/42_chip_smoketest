#include "button.h"

#include <stdint.h>
#include <util/delay.h>

#include "pin.h"

void press_release_event(const uint8_t pin, void (*f)(void*), void* arg) {
	if (is_pressed(SWITCH_PIN, pin)) {
		_delay_ms(20);
		if (is_pressed(SWITCH_PIN, pin))
			f(arg);
		while (is_pressed(SWITCH_PIN, pin));
	}
}

void init_btn(t_btn* btn, volatile uint8_t* port_ptr, uint8_t pin, t_btn_handler handler, void* handler_arg) {
	btn->port_ptr = port_ptr;
	btn->pin = pin;
	btn->count = 0;
	btn->state = 1;
	btn->handler = handler;
	btn->handler_arg = handler_arg;
}

void btn_poll(t_btn *btn) {
	if (read_pin(*btn->port_ptr, btn->pin) == btn->state)
		btn->count = 0;
	else
		++btn->count;
	if (btn->count == BTN_COUNT_THRESHOLD) {
		btn->state ^= 1;
		if (btn->state == 0)
			btn->handler(btn->handler_arg);
	}
}
