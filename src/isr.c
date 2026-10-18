#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "adc.h"
#include "button.h"
#include "display.h"
#include "ft_interrupt.h"
#include "pin.h"

// extern t_btn   s1_btn;
// extern t_btn   s2_btn;
// extern uint8_t mode;

volatile uint8_t g_ticks_2ms = 0;

FT_ISR(TIMER1_COMPA_vect)
{
	if (g_ticks_2ms != 255)
		++g_ticks_2ms;
}