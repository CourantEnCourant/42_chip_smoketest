#include "uart.h"

#include <avr/io.h>
#include <stdint.h>

#include "ft_interrupt.h"
#include "pin.h"
#include "rb.h"

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
