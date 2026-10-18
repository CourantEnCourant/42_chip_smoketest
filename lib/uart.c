#include "uart.h"

#include <avr/io.h>
#include <stdint.h>

#include "ft_interrupt.h"
#include "pin.h"
#include "rb.h"
#include "date.h"

#define SIZE 64

static void uart_tx_blocking(char c) {
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

static t_rb transmit_rb = {.f = uart_tx_blocking};

void uart_init(void) {
	// 20.2 power reduction register
	PRR &= ~(1 << PRUSART0);

	// 20.5 set baud rate
	UBRR0 = UBRR_VALUE;

	// 20.6.1 enable double speed async mode
	UCSR0A = (1 << U2X0);
	// enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	// 8 bit frame
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	// enable rx interrupt
	UCSR0B |= (1 << RXCIE0);
}

void uart_printstr(const char str[]) {
	while (*str)
		uart_tx(*str++);
}

char uart_rx(void) {
	while (bit_is_clear(UCSR0A, RXC0));
	return UDR0;
}

void uart_tx(char c) {
	uint8_t sreg = SREG;
	ft_cli();
	rb_push(&transmit_rb, c);
	// Kick the UDRE interrupt, the ISR drains the buffer
	set_bit(UCSR0B, UDRIE0);
	SREG = sreg;
}

void uart_tx_only_printable(char c) {
	// convention for screen tool is to apply \n with \r\n
	if (c == '\r')
		uart_printstr("\r\n");
	if (c >= 32 && c <= 126)
		uart_tx(c);
}

void print_2digit(uint8_t n) {
	uart_tx('0' + n / 10);
	uart_tx('0' + n % 10);
}

void uart_putuint(uint32_t n) {
	if (n < 10) {
		uart_tx('0' + n);
		return;
	}
	uart_putuint(n / 10);
	uart_tx('0' + n % 10);
}

FT_ISR(USART_UDRE_vect) {
	char c = rb_pop_front(&transmit_rb);
	if (c == '\0') {
		clear_bit(UCSR0B, UDRIE0);
		return;
	}
	UDR0 = c;
}

volatile uint8_t rx_complete = 0;

FT_ISR(USART_RX_vect) {
	rx_complete = 1;
}

static uint8_t uart_add_to_buf(char *buf, uint16_t size, uint16_t *i) {
	char c = UDR0;
	if (c == '\r' || *i == size - 1) {// end of input
		buf[*i] = '\0';
		*i = 0;
		return 1;
	} else if (c == 127 && *i > 0) {  // delete char
		uart_tx('\b');
		uart_tx(' ');
		uart_tx('\b');
		*i -= 1;
	} else if (32 <= c && c < 127) { // if printable
		buf[*i] = c;
		uart_tx(c);
		*i += 1;
	}
	return 0;
}

char buf[SIZE];
uint16_t i = 0;

void uart_input_routine(void) {
	if (rx_complete) {
		uint8_t input_complete = uart_add_to_buf(buf, SIZE, &i);
		rx_complete = 0;

		if (input_complete) {
			uart_printstr("\r\n");
			t_time t = {0};
			if (parse_time(buf, &t)) {
				set_time(&t);
			}
			uart_printstr("update date (dd/mm/yyyy hh:mm:ss) : ");
		}
	}
}