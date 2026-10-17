#include "timer.h"

#include "timer.h"

volatile uint8_t g_refresh = 0;

ISR(TIMER1_COMPA_vect)
{
	g_refresh = 1;
}

void init_timer1_int_2_ms(void)
{
	// power reduction reg
	PRR &= ~(1 << PRTIM1);

	TCNT1 = 0;
	OCR1A = 499;
	// OCR1A = 499; //  2 ms

	TIFR1 = (1 << OCF1A);
	TIMSK1 = (1 << OCIE1A);

	TCCR1B = (1 << WGM12); // mode 4 ctc
	TCCR1B |= (1 << CS11) | (1 << CS10);
}
