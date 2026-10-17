#include "main.h"

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>

#include "button.h"
#include "display.h"
#include "ft_interrupt.h"
#include "i2c.h"
#include "pin.h"
#include "uart.h"

static void led_setup() {
	set_bit(LED_DDR, D1);
	set_bit(LED_DDR, D2);
	set_bit(LED_DDR, D3);
	set_bit(LED_DDR, D4);
}

t_btn   s1_btn;
t_btn   s2_btn;
uint8_t mode = 0;

int main() {
	uart_init();
	i2c_init();
	if (!display_init())
		uart_printstr("display_init failed\r\n");

	led_setup();
	timer1_interrupt_init();
	init_btn(&s1_btn, &SWITCH_PIN, SW1, (t_btn_handler)sw1_press_handler, &mode, NULL, NULL);
	init_btn(&s2_btn, &SWITCH_PIN, SW2, (t_btn_handler)sw2_press_handler, &mode, NULL, NULL);
	ft_sei();
	while (1);
}
