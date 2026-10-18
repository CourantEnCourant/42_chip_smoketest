#include "main.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "adc.h"
#include "app.h"
#include "display.h"
#include "i2c.h"
#include "led.h"
#include "pca_async.h"
#include "uart.h"

extern volatile uint8_t g_ticks_2ms;

static t_btn g_sw1;
static t_btn g_sw2;
static t_btn g_sw3;

FT_ISR(TIMER1_COMPA_vect) {
	if (g_ticks_2ms != UINT8_MAX)
		++g_ticks_2ms;
}

// static uint8_t g_mode = 0;

static void buttons_init(void) {
	// direction in
	SWITCH_DDR &= ~(_BV(SW1) | _BV(SW2));

	// pull up
	SWITCH_PORT |= _BV(SW1) | _BV(SW2);

	init_btn(&g_sw1, &SWITCH_PIN, SW1, sw1_press_handler, 0, sw1_lift_handler, 0);
	init_btn(&g_sw2, &SWITCH_PIN, SW2, sw2_press_handler, 0, sw2_lift_handler, 0);
	init_btn(&g_sw3, 0, 0, sw3_press_handler, 0, sw3_lift_handler, 0);
}

static bool take_tick(void) {
	uint8_t saved_sreg = SREG;
	bool    ready;
	cli();
	ready = (g_ticks_2ms != 0);
	if (ready)
		--g_ticks_2ms;
	SREG = saved_sreg;
	return ready;
}

int main(void) {
	uart_init();
	i2c_init_async();
	pca_init_async();
	led_setup();
	adc_init();
	buttons_init();
	display_init();
	app_init();
	timer1_interrupt_init();
	ft_sei();
	uart_printstr("update date (dd/mm/yyyy hh:mm:ss) : ");
	while (1) {
		i2c_routine_async();
		pca_routine_async();
		uart_input_routine();
		if (take_tick()) {
			i2c_monitor_async();
			display_tick();
			app_tick_2ms();
			btn_poll_accessible(&g_sw1);
			btn_poll_accessible(&g_sw2);
			if (pca_ready_async()) {
				btn_poll(&g_sw3, pca_sw3_pressed_async() ? PRESSED : LIFTED);
			}
			// pca_single_led_set_async(1, 1);
			// pca_write_cathodes_async(0);
		}
		// before display_service: a mode collects its i2c result
		// before the display can start the next frame
		app_service();
		display_service();
	}
	return 0;
}
