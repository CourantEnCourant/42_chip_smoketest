#include "uart.h"

#include <avr/io.h>

void uart_printstr(const char str[]) {
	while (*str)
		uart_tx(*str++);
}

char uart_rx() {
	while (bit_is_clear(UCSR0A, RXC0));
	return UDR0;
}

void uart_tx(char c) {
	while (bit_is_clear(UCSR0A, UDRE0));
	UDR0 = c;
}
