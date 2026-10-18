#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "button.h"
#include "ft_interrupt.h"

extern t_btn            s1_btn;
extern t_btn            s2_btn;
extern volatile uint8_t mode;

volatile uint32_t ticks;

FT_ISR(TIMER1_COMPA_vect) {
	++ticks;
	btn_poll_accessible(&s1_btn);
	btn_poll_accessible(&s2_btn);
}
