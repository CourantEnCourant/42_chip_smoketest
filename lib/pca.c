#include "pca.h"

#include <stdint.h>
#include <util/twi.h>

#include "i2c.h"
#include "uart.h"

bool pca_start_sequence(uint8_t rw) {
	// START
	i2c_start();
	// i2c_print_status(); // expect 0x08
	if (i2c_get_status() != TW_START && i2c_get_status() != TW_REP_START)
		return (uart_printstr("pca_start_sequence error: 1\r\n"), i2c_stop(), false);

	// SLA + R/W
	i2c_init_com(PCA_ADDR, rw);
	// i2c_print_status(); // expect 0x18
	if (i2c_get_status() != TW_MT_SLA_ACK && i2c_get_status() != TW_MR_SLA_ACK)
		return (uart_printstr("pca_start_sequence error: 2\r\n"), i2c_stop(), false);

	return (true);
}

bool pca_write(uint8_t reg, uint8_t value) {
	if (!pca_start_sequence(WRITE))
		return (uart_printstr("pca_write error: 1\r\n"), false);

	i2c_write(reg);
	// i2c_print_status(); // expect 0x28
	if (i2c_get_status() != TW_MT_DATA_ACK)
		return (uart_printstr("pca_write error: 2\r\n"), i2c_stop(), false);

	i2c_write(value);  // its are high so we need to send inversion of avr standard
	// i2c_print_status(); // expect 0x28
	if (i2c_get_status() != TW_MT_DATA_ACK)
		return (uart_printstr("pca_write error: 3\r\n"), i2c_stop(), false);

	// uart_printstr("pca_write success\r\n");

	i2c_stop();

	return (true);
}

bool pca_read(uint8_t reg, uint8_t* value) {
	uint8_t data;

	// start
	if (!pca_start_sequence(WRITE))
		return (uart_printstr("pca_read error: 1\r\n"), false);

	// send adress we ant to read
	i2c_write(reg);
	// i2c_print_status(); // expect 0x28
	if (i2c_get_status() != TW_MT_DATA_ACK)
		return (uart_printstr("pca_read error: 2\r\n"), i2c_stop(), false);

	// rep start
	if (!pca_start_sequence(READ))
		return (uart_printstr("pca_read error: 3\r\n"), false);

	data = i2c_read_byte(0);  // nack requested for last byte
	// i2c_print_status(); // 0x50
	if (i2c_get_status() != TW_MR_DATA_NACK)
		return (uart_printstr("pca_read error:  4\r\n"), i2c_stop(), false);

	// uart_printstr("pca_read success\r\n");

	*value = data;  // change ptr at end of segment for no change if false is returned

	i2c_stop();

	return (true);
}

bool pca_set_bits(uint8_t reg, uint8_t bits) {
	uint8_t data;

	if (!pca_read(reg, &data))
		return (uart_printstr("pca_add_to_reg error: 1\r\n"), false);

	data |= bits;

	if (!pca_write(reg, data))
		return (uart_printstr("pca_add_to_reg error: 2\r\n"), false);

	return (true);
}

bool pca_clear_bits(uint8_t reg, uint8_t bits) {
	uint8_t data;

	if (!pca_read(reg, &data))
		return (uart_printstr("pca_rmv_to_reg error: 1\r\n"), false);

	data &= ~bits;

	if (!pca_write(reg, data))
		return (uart_printstr("pca_rmv_to_reg error: 2\r\n"), false);

	return (true);
}
