#include "i2c.h"

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
uint8_t compute_bit_rate(uint32_t f_cpu, uint32_t f_scl, uint8_t prescaler)
{
	return ((f_cpu - 16 * f_scl) / (2 * f_scl * prescaler));
}

void i2c_init(void)
{
	// 22.5.1 pull up resistor
	PORTC |= (1 << PC4) | (1 << PC5);

	// 22.5.2 set bit rate
	TWBR = compute_bit_rate(F_CPU, F_SCL, SCL_PRESCALER);

	// default prescaler to 1

	// 22.9.2 activate enable TWI on Control reg
	TWCR |= (1 << TWEN);
}

// page 225 22.6 code examples
// eg  2
void i2c_wait_twint(void)
{
	while (!(TWCR & (1 << TWINT)))
		;
}

// page 225 22.6 code examples
// eg 1 and 2 send start, wait for  twijnt flag to be setted, meaning the succeffull transmition
void i2c_start(void)
{
	// 1 TWINT	setting twint will erase it
	// 2 TWSTA	will exec Start Call internally 
	// 3 TWEN	Enable TWI
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	// wait for twint flag to be set again (from 0 to 1)
	// this will indicate that the last call have been transmitted
	i2c_wait_twint();
}

// page 225 22.6 code examples
void i2c_stop(void)
{
	// 1 TWINT	setting twint will erase it
	// 2 TWSTO	will exec Stop Call internally 
	// 3 TWEN	Enable TWI
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

	// wait for twint flag to be set again (from 0 to 1)
	// this will indicate that the last call have been transmitted
	// i2c_wait_twint();
}

/*
	will perform SLA + Read/Write (Slave Adress and req type)

	addr must be 7 bit max (126) and will be shifted of 1 into TWDR
	rw must be set to 0 for write and 1 for read
*/
void i2c_init_com(uint8_t addr, uint8_t rw)
{
	// holds the data SLA + R/W
	TWDR = ((addr << 1) | rw);

	// unset TWINT will initiate
	// an ISR the sla will be sent and the flag 
	// will be resetted at the end
	TWCR = (1 << TWINT) | (1 << TWEN);

	// wait for end of transmission
	i2c_wait_twint();
}

uint8_t	i2c_get_status(void)
{
	// wait last action over
	i2c_wait_twint();

	// TWSR is tricky only the last 5 bits are relevant for the status
	// equivalent to TWSR & 0b11111000 
	return (TWSR & TW_STATUS_MASK);
}

void i2c_print_status(void)
{
	uart_printstr("status: ");

	print_hex_value_u8(i2c_get_status());

	uart_printstr("\r\n");
}

/*
	will scan all 126 addresses
	and ouput the ones open on the chip
*/
void i2c_scan(void)
{
	uint8_t address = 1;
    while (address < 128)
	{
        // START
		i2c_start();

        // Send SLA 
		i2c_init_com(address, WRITE); // MT

        // if slave ACK
        if (i2c_get_status() == TW_MT_SLA_ACK)
		{
            print_hex_value_u8(address);
			uart_printstr("\r\n");
		}

        // STOP
        i2c_stop();

		address++;
    }
}

void	i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait_twint();
}

uint8_t	i2c_read_byte(uint8_t ack)
{
	TWCR = (1 << TWEN);

	if (ack)
		TWCR |= (1 << TWEA); // enable ACK response
	
	TWCR |= (1 << TWINT); // must be done at the end to start the process

	i2c_wait_twint();

	return (TWDR);
}

void	i2c_read(void)
{
	uint8_t value = i2c_read_byte(1);

	print_hex_value(value);
	uart_printstr("\r\n");
}