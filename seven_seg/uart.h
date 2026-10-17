#ifndef UART_H
# define UART_H

# include <avr/io.h>
# include <util/delay.h>
# include <util/twi.h>
# include <avr/interrupt.h>

// error percent ~ 2.1 
// page 199 table 20-7
// for async double speed mode 
/*
		 		F_CPU
	UBRR =	------------    - 1
			8 * BAUD_RATE

			give us 17.36 ... - 1 = 16.36 ...

			~ 16
*/
# define UBRR ((F_CPU / (BAUD_RATE * 8)) - 1) // = 16

void uart_printstr(const char* str);
void uart_init(uint16_t ubrr);
void uart_tx( char data );
char uart_rx(void);
void uart_tx_only_printable(char c);

#endif