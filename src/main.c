#include "main.h"

#include <avr/io.h>
#include <stddef.h>

#include "button.h"
#include "ft_interrupt.h"
#include "pin.h"

static void led_setup() {
	set_bit(LED_DDR, D1);
}

static void timer1_interrupt_init() {
	// power reduction reg
	PRR &= ~(1 << PRTIM1);

	TCNT1 = 0;
	OCR1A = 499;
	// OCR1A = 499; //  2 ms

	TIFR1 = (1 << OCF1A);
	TIMSK1 = (1 << OCIE1A);

	TCCR1B = (1 << WGM12);  // mode 4 ctc
	TCCR1B |= (1 << CS11) | (1 << CS10);
}

t_btn s1_btn;

int main() {
	led_setup();
	timer1_interrupt_init();
	init_btn(&s1_btn, &SWITCH_PIN, SW1, sw1_press_handler, NULL, sw1_lift_handler, NULL);
	ft_sei();
	while (1);
}
