#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "ft_interrupt.h"
#include "button.h"
#include "pca_async.h"

volatile uint8_t g_ticks_2ms = 0;


// FT_ISR(TIMER1_COMPA_vect)
// {
// 	btn_poll_accessible(&g_sw1);
// 	btn_poll_accessible(&g_sw2);
//     if (pca_async_ready())
//         btn_poll_sample(&g_sw3, pca_sw3_pressed() ? PRESSED : LIFTED);

// 	if (g_ticks_2ms != UINT8_MAX)
// 		++g_ticks_2ms;
// }
