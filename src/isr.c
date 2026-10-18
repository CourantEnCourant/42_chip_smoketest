#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "ft_interrupt.h"

volatile uint8_t g_ticks_2ms = 0;

FT_ISR(TIMER1_COMPA_vect) {
	if (g_ticks_2ms != UINT8_MAX)
		++g_ticks_2ms;
}
