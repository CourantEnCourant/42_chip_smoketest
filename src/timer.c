#include <avr/io.h>
#include "main.h"
#include "ft_interrupt.h"

void timer1_interrupt_init(void)
{
	PRR &= (uint8_t)~_BV(PRTIM1);
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// 2ms
	OCR1A = (F_CPU / (64UL * 500UL)) - 1UL;
	TIFR1 = _BV(OCF1A);
	TIMSK1 = _BV(OCIE1A);
	TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);
}
