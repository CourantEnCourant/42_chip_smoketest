#pragma once

#include <avr/io.h>

void uart_printstr(const char str[]);
char uart_rx();
void uart_tx(char c);

#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#define UART_BAUDRATE 115200
#define UBRR_VALUE    (F_CPU / (8 * UART_BAUDRATE) - 1)
#define set_ubrr() UBRR0H = UBRR_VALUE >> 8; \
							UBRR0L = UBRR_VALUE;
