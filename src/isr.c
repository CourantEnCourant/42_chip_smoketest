#include <avr/interrupt.h>

#include "button.h"
#include "ft_interrupt.h"

extern t_btn s1_btn;

FT_ISR(TIMER1_COMPA_vect) {
	btn_poll(&s1_btn);
}
