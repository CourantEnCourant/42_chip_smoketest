#include "adc.h"

#include <avr/io.h>
#include <stdint.h>

#include "pin.h"

void adc_init() {
	// 10 bit representation
	// Prescaler 128
	set_bit(ADCSRA, ADPS2);
	set_bit(ADCSRA, ADPS1);
	set_bit(ADCSRA, ADPS0);
	// ADC Enable
	set_bit(ADCSRA, ADEN);
}

uint16_t get_adc() {
	set_bit(ADCSRA, ADSC);
	loop_until_bit_is_clear(ADCSRA, ADSC);
	return ADC;
}
