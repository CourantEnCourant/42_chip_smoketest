#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "adc.h"
#include "button.h"
#include "display.h"
#include "ft_interrupt.h"
#include "pin.h"

extern t_btn   s1_btn;
extern t_btn   s2_btn;
extern uint8_t mode;

FT_ISR(TIMER1_COMPA_vect) {
	btn_poll(&s1_btn);
	btn_poll(&s2_btn);
	if (mode == 0) {
		// Ref voltage on AVCC
		clear_bit(ADMUX, REFS1);
		set_bit(ADMUX, REFS0);
		// RV1
		clear_bit(ADMUX, MUX3);
		clear_bit(ADMUX, MUX1);
		clear_bit(ADMUX, MUX0);
		display_set_number(get_adc());
	} else if (mode == 1) {
		// Ref voltage on AVCC
		clear_bit(ADMUX, REFS1);
		set_bit(ADMUX, REFS0);
		// LDR
		clear_bit(ADMUX, MUX3);
		clear_bit(ADMUX, MUX1);
		set_bit(ADMUX, MUX0);
		display_set_number(get_adc());
	} else if (mode == 2) {
		// Ref voltage on AVCC
		clear_bit(ADMUX, REFS1);
		set_bit(ADMUX, REFS0);
		// NTC
		clear_bit(ADMUX, MUX3);
		set_bit(ADMUX, MUX1);
		clear_bit(ADMUX, MUX0);
		display_set_number(get_adc());
	} else if (mode == 3) {
		// Internal 1.1V ref
		set_bit(ADMUX, REFS1);
		set_bit(ADMUX, REFS0);
		// Temperature mesurement
		set_bit(ADMUX, MUX3);
		clear_bit(ADMUX, MUX1);
		clear_bit(ADMUX, MUX0);
		display_set_number((get_adc() - T_OS) / K);
	} else
		display_set_number(8888);
	display_refresh();
}
