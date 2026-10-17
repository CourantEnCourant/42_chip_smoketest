#include "print.h"

#include <stdint.h>

#include "uart.h"

static const char hex[] = "0123456789ABCDEF";

void print_dec_value_u32(uint32_t dw) {
	if (dw >= 10)
		print_dec_value_u32(dw / 10);
	uart_tx(dw % 10 + '0');
}

void print_dec_value_i32(int32_t dw) {
	if (dw < 0) {
		uart_tx('-');
		// unsigned negation is safe for INT32_MIN
		print_dec_value_u32(-(uint32_t)dw);
	} else
		print_dec_value_u32((uint32_t)dw);
}

void print_dec_value_u16(uint16_t w) {
	print_dec_value_u32(w);
}

void print_dec_value_i16(int16_t w) {
	print_dec_value_i32(w);
}

void print_hex_value(char c) {
	uint8_t uc = (uint8_t)c;

	uart_tx(hex[uc >> 4]);
	uart_tx(hex[uc & 0x0F]);
}

void print_hex_value_u8(uint8_t c) {
	uart_printstr("0x");
	print_hex_value((char)c);
}

void print_hex_value_u12(uint16_t w) {
	uart_printstr("0x");
	uart_tx(hex[(w >> 8) & 0x0F]);
	print_hex_value((char)w);
}

void print_hex_value_u16(uint16_t w) {
	uart_printstr("0x");
	print_hex_value((char)(w >> 8));
	print_hex_value((char)w);
}
