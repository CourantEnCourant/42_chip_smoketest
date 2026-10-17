#include "main.h"

#include <avr/io.h>
#include <stddef.h>

#include "button.h"
#include "ft_interrupt.h"
#include "pin.h"

static void led_setup() {
	set_bit(LED_DDR, D1);
}

t_btn s1_btn;

int main() {
	led_setup();
	timer1_interrupt_init();
	init_btn(&s1_btn, &SWITCH_PIN, SW1, sw1_press_handler, NULL, sw1_lift_handler, NULL);
	ft_sei();
	while (1);
}
