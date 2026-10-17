#include "uart.h"

void print_dec_value_u32(uint32_t dw)
{
	if (dw < 10)
		uart_tx(dw + '0');
	else
	{
		print_dec_value_u32(dw / 10);
		print_dec_value_u32(dw % 10);
	}
}

void print_dec_value_i32(uint32_t dw)
{
	if (dw == -2147483648)
	{
		uart_printstr("-2147483648");
		return ;
	}

	if (dw < 0)
	{
		dw = -dw;
		uart_tx('-');
	}

	print_dec_value_u32((uint32_t)dw);
}

void print_dec_value_u16(uint16_t w)
{
	if (w < 10)
		uart_tx(w + '0');
	else
	{
		print_dec_value_u16(w / 10);
		print_dec_value_u16(w % 10);
	}
}

void print_dec_value_i16(int16_t w)
{
	if (w == -65536)
	{
		uart_printstr("-65536");
		return ;
	}

	if (w < 0)
	{
		w = -w;
		uart_tx('-');
	}

	print_dec_value_u16((uint16_t)w);
}

// really reptitive but fits the subject
void print_hex_value(char c)
{
	const char *hex = "0123456789ABCDEF";
	uint8_t buf[3];

	uint8_t uc = (uint8_t)c;

	buf[0] = hex[uc >> 4];
	buf[1] = hex[uc & 0b00001111];
	buf[2] = '\0';

	uart_printstr(buf);
}

void print_hex_value_u8(uint8_t c)
{
	const char *hex = "0123456789ABCDEF";
	uint8_t buf[5];

	buf[0] = '0';
	buf[1] = 'x';
	buf[2] = hex[c >> 4];
	buf[3] = hex[c & 0b00001111];
	buf[4] = '\0';

	uart_printstr(buf);
}

void print_hex_value_u12(uint16_t w)
{
	const char *hex = "0123456789ABCDEF";
	uint8_t buf[6];

	buf[0] = '0';
	buf[1] = 'x';
	buf[2] = hex[(w & 0b0000111100000000) >> 8];
	buf[3] = hex[(w & 0b0000000011110000) >> 4];
	buf[4] = hex[(w & 0b0000000000001111)];
	buf[5] = '\0';

	uart_printstr(buf);
}

void print_hex_value_u16(uint16_t w)
{
	const char *hex = "0123456789ABCDEF";
	uint8_t buf[7];

	buf[0] = '0';
	buf[1] = 'x';
	buf[2] = hex[(w & 0b1111000000000000) >> 12];
	buf[3] = hex[(w & 0b0000111100000000) >> 8];
	buf[4] = hex[(w & 0b0000000011110000) >> 4];
	buf[5] = hex[(w & 0b0000000000001111)];
	buf[6] = '\0';

	uart_printstr(buf);
}


void uart_init(uint16_t ubrr)
{
	// 20.2 power reduction register
	PRR &= ~(1 << PRUSART0);

	// 20.5 set baud rate 
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	// 20.6.1 enable double speed async mode
	// purposed override
	UCSR0A = (1 << U2X0);

	// schema page 185 : enable receiver and transmitter 
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// set UCSZn mode 8 bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/* doc 20.6.1 */
void uart_tx( char data )
{
	// Polling of UDRE0 (UART Data Register Empty) Flag
	while (!( UCSR0A & (1 << UDRE0)))
		;

	// Put data UDR (UART Data Register)
	UDR0 = data;
}

/* doc 20.7.1 */
char uart_rx(void)
{
	// Polling of RXC0 (Receive Complete) Flag
	while (!(UCSR0A & (1 << RXC0)))
		;
	
	// return copy of UDR (UART Data Register)
	return (UDR0);
}

void uart_tx_only_printable(char c)
{

	// convention for screen tool is to apply \n with \r\n
	if (c == '\r')
		uart_printstr("\r\n");
	// only printable character
	if (c >= 32 && c <= 126)
		uart_tx(c);
}

void uart_printstr(const char* str)
{
	uint16_t i = 0;

	while (str[i])
		uart_tx(str[i++]);
}