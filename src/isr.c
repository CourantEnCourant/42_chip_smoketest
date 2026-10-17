#include <avr/interrupt.h>

#include "button.h"
#include "display.h"
#include "ft_interrupt.h"

extern t_btn s1_btn;
extern t_btn s2_btn;

FT_ISR(TIMER1_COMPA_vect) {
	btn_poll(&s1_btn);
	btn_poll(&s2_btn);
	display_refresh();
}
