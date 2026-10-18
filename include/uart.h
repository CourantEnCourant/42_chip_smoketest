#pragma once

#include <avr/io.h>

#ifndef F_CPU
# define F_CPU 16000000UL
#endif

// async double speed mode (U2X0), table 20-7: error ~2.1%
#define UART_BAUDRATE 115200
#define UBRR_VALUE    (F_CPU / (8 * UART_BAUDRATE) - 1)

void uart_init(void);
void uart_printstr(const char str[]);
char uart_rx(void);
void uart_tx(char c);
void uart_tx_only_printable(char c);
void print_2digit(uint8_t n);
void uart_putuint(uint32_t n);
