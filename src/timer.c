#include <avr/io.h>

#include "main.h"

void timer1_interrupt_init() {
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
