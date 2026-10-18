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

void init_btn(t_btn*            btn,
              volatile uint8_t* port_ptr,
              uint8_t           pin,
              t_btn_handler     press_handler,
              void*             press_arg,
              t_btn_handler     lift_handler,
              void*             lift_arg) {
	btn->port_ptr = port_ptr;
	btn->pin = pin;
	btn->count = 0;
	btn->state = LIFTED;
	btn->press_handler = press_handler;
	btn->press_arg = press_arg;
	btn->lift_handler = lift_handler;
	btn->lift_arg = lift_arg;
}

void btn_poll(t_btn* btn, uint8_t pin_state)
{
	if (pin_state == btn->state)
	{
		btn->count = 0;
		return ;
	}
	if (btn->count < BTN_COUNT_THRESHOLD)
	{
		++btn->count;
		return ;
	}
	// case btn_count == BTN_TRESHOLD
	btn->state = pin_state;
	btn->count = 0;
	if (btn->state == PRESSED && btn->press_handler)
		btn->press_handler(btn->press_arg);
	if (btn->state == LIFTED && btn->lift_handler)
		btn->lift_handler(btn->lift_arg);
}

void btn_poll_accessible(t_btn* btn)
{
	btn_poll(btn, read_pin(*btn->port_ptr, btn->pin));
}
