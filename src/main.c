#include "main.h"
#include "display.h"
#include "i2c.h"
#include "pca_async.h"
#include "uart.h"
#include "button.h"
#include "pin.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// defined in src/isr.c
extern volatile uint8_t g_ticks_2ms;

/*
	very important
	atomic op

	must be called to remove a tick from the queue

	as the isr is only launching tick continuously
*/
static bool take_tick(void)
{
	uint8_t sreg = SREG;
	bool ready;

	cli();

	ready = (g_ticks_2ms != 0);

	if (ready)
		--g_ticks_2ms;

	SREG = sreg;
	return ready;
}

static void led_setup() {
	set_bit(DDRB, PB0);
	set_bit(DDRB, PB1);
	set_bit(DDRB, PB2);
	set_bit(DDRB, PB4);
}

t_btn   g_sw1;
t_btn   g_sw2;
uint8_t mode = 0;

/* BUTTONS INIT */

static void buttons_init(void)
{
	// DDR entry
	DDRD &= ~(_BV(PD2) | _BV(PD4));

	// pull up
	PORTD |= _BV(PD2) | _BV(PD4);

	init_btn(&g_sw1, &PIND, PD2, (t_btn_handler)sw1_press_handler, 0, sw1_lift_handler, 0);

	init_btn(&g_sw2, &PIND, PD4, (t_btn_handler)sw2_press_handler, 0, sw2_lift_handler, 0);
}

int main(void)
{
	uint8_t error_reported = 0;

	uart_init();
	
	i2c_init_async();
	pca_init_async();

	// test
	led_setup();
	buttons_init();

	// this desnt display it just stores the next number
	display_init();
	display_set_number(1234);

	timer1_interrupt_init();
	
	ft_sei();

	while (1)
	{
		// continuaous actions
		// non blocking update
		i2c_routine_async();
		pca_routine_async();
		display_service();

		// periodic actions
		// each 2 ms
		if (take_tick())
		{
			// check timeout
			i2c_tick();

			// button poll
			btn_poll_accessible(&g_sw1);
			btn_poll_accessible(&g_sw1);

			// ask next number
			display_tick();
		}

		// debug
		if (!error_reported && (pca_failed_async() || display_has_error()))
		{
			uart_printstr("DISPLAY ERROR\r\n");
			error_reported = 1;
		}
	}
}

