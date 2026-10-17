#include <avr/interrupt.h>
#include <stdint.h>

#include "adc.h"
#include "button.h"
#include "display.h"
#include "ft_interrupt.h"

extern t_btn   s1_btn;
extern t_btn   s2_btn;
extern uint8_t mode;

FT_ISR(TIMER1_COMPA_vect) {
	btn_poll(&s1_btn);
	btn_poll(&s2_btn);
	if (mode == 0)
		display_set_number(get_adc());
	display_refresh();
}
