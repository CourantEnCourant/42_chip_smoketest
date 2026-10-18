#include "main.h"

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>

#include "adc.h"
#include "button.h"
#include "display.h"
#include "ft_interrupt.h"
#include "i2c.h"
#include "led.h"
#include "pin.h"
#include "uart.h"

t_btn   s1_btn;
t_btn   s2_btn;
uint8_t mode = 0;

int main() {
	uart_init();
	i2c_init();
	if (!display_init())
		uart_printstr("display_init failed\r\n");

	led_setup();
	adc_init();
	timer1_interrupt_init();
	init_btn(&s1_btn, &SWITCH_PIN, SW1, (t_btn_handler)sw1_press_handler, &mode, NULL, NULL);
	init_btn(&s2_btn, &SWITCH_PIN, SW2, (t_btn_handler)sw2_press_handler, &mode, NULL, NULL);
	ft_sei();
	while (1) {
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
		} else
			display_set_number(8888);
		display_refresh();
	}
}
