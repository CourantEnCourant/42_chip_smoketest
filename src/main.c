#include "main.h"

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>

#include "adc.h"
#include "button.h"
#include "date.h"
#include "display.h"
#include "ft_interrupt.h"
#include "i2c.h"
#include "led.h"
#include "pin.h"
#include "uart.h"

t_btn                    s1_btn;
t_btn                    s2_btn;
volatile uint8_t         mode = 0;
extern volatile uint32_t ticks;

static uint32_t get_ticks_atomist(void) {
	uint8_t  sreg = SREG;
	uint32_t t;
	ft_cli();
	t = ticks;
	SREG = sreg;
	return t;
}
#define SIZE 64

static void led_setup() {
	set_bit(LED_DDR, D1);
	set_bit(LED_DDR, D2);
	set_bit(LED_DDR, D3);
	set_bit(LED_DDR, D4);
}

volatile uint8_t rx_complete = 0;

FT_ISR(USART_RX_vect) {
	rx_complete = 1;
}

t_btn   s1_btn;
t_btn   s2_btn;
uint8_t mode = 0;

int main() {
	uint8_t input_complete = 0;

	uart_init();
	i2c_init();
	if (!display_init())
		uart_printstr("display_init failed\r\n");

	led_setup();
	adc_init();
	timer1_interrupt_init();
	init_btn(&s1_btn, &SWITCH_PIN, SW1, (t_btn_handler)sw1_press_handler, (void*)&mode, NULL, NULL);
	init_btn(&s2_btn, &SWITCH_PIN, SW2, (t_btn_handler)sw2_press_handler, (void*)&mode, NULL, NULL);
	ft_sei();

	enum e_main_state main_state = INIT;
	t_time            t;
	display_set_number(8888);
	while (1) {
		if (main_state == INIT) {
			uint32_t t = get_ticks_atomist();
			if (t >= 2000) {
				main_state = HOTPATH;
				led_display_mode(mode);
				continue;
			}
			if (t < 1500) {
				led_all_lights_on();
				display_refresh();
			} else
				led_all_lights_off();
			continue;
		}
		if (mode == 0) {
			// Ref voltage on AVCC
			clear_bit(ADMUX, REFS1);
			set_bit(ADMUX, REFS0);
			// RV1
			clear_bit(ADMUX, MUX3);
			clear_bit(ADMUX, MUX1);
			clear_bit(ADMUX, MUX0);
			display_set_number(get_adc());
		} else if (mode == 1) {
			// Ref voltage on AVCC
			clear_bit(ADMUX, REFS1);
			set_bit(ADMUX, REFS0);
			// LDR
			clear_bit(ADMUX, MUX3);
			clear_bit(ADMUX, MUX1);
			set_bit(ADMUX, MUX0);
			display_set_number(get_adc());
		} else if (mode == 2) {
			// Ref voltage on AVCC
			clear_bit(ADMUX, REFS1);
			set_bit(ADMUX, REFS0);
			// NTC
			clear_bit(ADMUX, MUX3);
			set_bit(ADMUX, MUX1);
			clear_bit(ADMUX, MUX0);
			display_set_number(get_adc());
		} else if (mode == 3) {
			// Internal 1.1V ref
			set_bit(ADMUX, REFS1);
			set_bit(ADMUX, REFS0);
			// Temperature mesurement
			set_bit(ADMUX, MUX3);
			clear_bit(ADMUX, MUX1);
			clear_bit(ADMUX, MUX0);
			display_set_number((get_adc() - T_OS) / K);
		} else if (mode == 8) {
			get_time(&t);
			display_set_number(t.hour * 100 + t.min);
		} else if (mode == 9) {
			get_time(&t);
			display_set_number(t.day * 100 + t.month);
		} else if (mode == 10) {
			get_time(&t);
			display_set_number(2000 + t.year);
		} else
			display_set_number(8888);
		display_refresh();
	uart_printstr("input date :");
	while (1) {
		if (rx_complete) {
			char c = UDR0;
			if (c == '\r' || i == SIZE - 1) {// end of input
				buf[i] = '\0';
				input_complete = 1;
			} else if (c == 127 && i > 0) {  // delete char
				uart_tx('\b');
				uart_tx(' ');
				uart_tx('\b');
				i--;
			} else if (32 <= c && c < 127) { // if printable
				buf[i++] = c;
				uart_tx(c);
			}
		}
	}
}
