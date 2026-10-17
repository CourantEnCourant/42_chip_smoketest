#ifndef I2C_H
# define I2C_H

# include <avr/io.h>
# include <util/delay.h>
# include <util/twi.h>
# include <avr/interrupt.h>

# include "uart.h"

#define F_SCL			100000UL // 100KHz
#define SCL_PRESCALER	1		 // def

#define WRITE			0
#define READ			1

void	i2c_init(void);
void	i2c_start(void);
void	i2c_stop(void);

uint8_t compute_bit_rate(uint32_t f_cpu, uint32_t f_scl, uint8_t prescaler);
void	i2c_wait_twint(void);
void	i2c_init_com(uint8_t addr, uint8_t rw);
uint8_t	i2c_get_status(void);
void	i2c_print_status(void);
void	i2c_scan(void);

void	i2c_write(unsigned char data);
uint8_t	i2c_read_byte(uint8_t ack);
void	i2c_read(void);

void	aht20_connexion_attempt(void);

/* USE UART */

void print_hex_value(char c);
void print_hex_value_u12(uint16_t w);
void print_hex_value_u16(uint16_t w);
void print_hex_value_u8(uint8_t c);
void print_dec_value_u16(uint16_t w);
void print_dec_value_i16(int16_t w);
void print_dec_value_u32(uint32_t dw);
void print_dec_value_i32(uint32_t dw);

#endif