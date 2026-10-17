#ifndef TIMER_H
# define TIMER_H

# include <avr/io.h>
# include <util/delay.h>
# include <util/twi.h>
# include <avr/interrupt.h>

# include "pca.h"

# define TIMER_TOP	F_CPU / 1000 * 2 // 2 ms (no prescaling) 32000 (fits uint16)

void init_timer1_int_2_ms(void);

extern volatile uint8_t g_refresh;

#endif