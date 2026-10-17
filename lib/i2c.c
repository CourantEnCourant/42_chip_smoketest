#include "i2c.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/twi.h>

#include "print.h"
#include "uart.h"

/*
J AI GALERE A TROUVER LA FORMULE POUR SET LE BIT_RATE
trouver sur cette page un algo focntoinel : https://www.idreammicro.com/post/Configuration-du-Bit-Rate-Generator-Unit

il y a un shema suivant

        F_CPU - (16 * F_SCL)
TWBR =	------------------
        2 * F_SCL * PRESCALER

il nous disent que la doc atm nous invite a privilegier un PreScaler a 1
mais j ai pas trouve de preuve dans la doc
*/
uint8_t compute_bit_rate(uint32_t f_cpu, uint32_t f_scl, uint8_t prescaler) {
	return (f_cpu - 16 * f_scl) / (2 * f_scl * prescaler);
}

void i2c_init(void) {
	// 22.5.1 pull up resistor
	PORTC |= (1 << PC4) | (1 << PC5);

	// 22.5.2 set bit rate (default prescaler to 1)
	TWBR = compute_bit_rate(F_CPU, F_SCL, SCL_PRESCALER);

	// 22.9.2 activate enable TWI on Control reg
	TWCR |= (1 << TWEN);
}

// page 225 22.6 code examples, eg 2
void i2c_wait_twint(void) {
	while (!(TWCR & (1 << TWINT)));
}

// page 225 22.6 code examples, eg 1 and 2
// send start, wait for twint flag to be set, meaning the successful transmission
void i2c_start(void) {
	// TWINT: setting twint will erase it
	// TWSTA: will exec Start Call internally
	// TWEN:  Enable TWI
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	i2c_wait_twint();
}

// page 225 22.6 code examples
void i2c_stop(void) {
	// TWSTO: will exec Stop Call internally
	// TWINT is not set after a STOP, so no wait here
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/*
    will perform SLA + Read/Write (Slave Address and req type)

    addr must be 7 bit max and will be shifted of 1 into TWDR
    rw must be set to 0 for write and 1 for read
*/
void i2c_init_com(uint8_t addr, uint8_t rw) {
	TWDR = (addr << 1) | rw;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait_twint();
}

uint8_t i2c_get_status(void) {
	i2c_wait_twint();
	// only the 5 upper bits of TWSR are relevant for the status
	return TWSR & TW_STATUS_MASK;
}

void i2c_print_status(void) {
	uart_printstr("status: ");
	print_hex_value_u8(i2c_get_status());
	uart_printstr("\r\n");
}

// scan all 7-bit addresses and output the ones answering
void i2c_scan(void) {
	for (uint8_t address = 1; address < 128; ++address) {
		i2c_start();
		i2c_init_com(address, WRITE);
		if (i2c_get_status() == TW_MT_SLA_ACK) {
			print_hex_value_u8(address);
			uart_printstr("\r\n");
		}
		i2c_stop();
	}
}

void i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait_twint();
}

uint8_t i2c_read_byte(uint8_t ack) {
	TWCR = (1 << TWEN);
	if (ack)
		TWCR |= (1 << TWEA);  // enable ACK response
	TWCR |= (1 << TWINT);     // must be done at the end to start the process
	i2c_wait_twint();
	return TWDR;
}

void i2c_read(void) {
	print_hex_value(i2c_read_byte(1));
	uart_printstr("\r\n");
}
