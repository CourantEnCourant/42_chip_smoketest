#pragma once

#include <avr/io.h>
#include <util/twi.h>

#include <stdint.h>
#include <stdbool.h>

#include "ft_interrupt.h"

#define F_SCL         100000UL  // 100KHz
#define SCL_PRESCALER 1

#define WRITE         0
#define READ          1

uint8_t compute_bit_rate(uint32_t f_cpu, uint32_t f_scl, uint8_t prescaler);

/* 
	global data use for the async i2c process wil lpreserve 
	data and act as a state machine each time called in the main via i2c_routine_async 
*/

typedef enum	enum_i2c_phase
{
	PHASE_I2C_IDLE,
	PHASE_I2C_START,
	PHASE_I2C_ADDR_W,
	PHASE_I2C_ADDR_R,
	PHASE_I2C_REGISTER,
	PHASE_I2C_RESTART,
	PHASE_I2C_READ_BYTE,
	PHASE_I2C_WRITE_BYTE,
	PHASE_I2C_STOP
}				e_i2c_phase;

typedef enum	enum_i2c_result
{
	RES_I2C_IDLE,
	RES_I2C_BUSY,
	RES_I2C_OK,
	RES_I2C_ERROR
}				e_i2c_result;

typedef struct		s_i2c
{
	e_i2c_phase		phase;
	e_i2c_result	result;
	uint8_t			status;
	uint8_t			data;

	uint8_t			addr;
	uint8_t			reg;
	uint8_t			value;
	uint8_t			is_read;

	uint8_t			sync_active;
	uint8_t			timeout;
}					t_i2c;

/* async non blocking interface */

void			i2c_init_async(void);
bool			i2c_write_async(uint8_t addr, uint8_t reg, uint8_t value);
bool			i2c_read_async(uint8_t addr, uint8_t reg);
// try to call as much as possible to ensure the flow
void			i2c_routine_async(void);
// check progress
void			i2c_monitor_async(void);
// utils
bool			i2c_is_busy_async(void);
e_i2c_result	i2c_result_async(void);
uint8_t			i2c_status_async(void);
uint8_t			i2c_data_async(void);




/* legacy blocking interface */
void    i2c_start(void);
void    i2c_stop(void);
void    i2c_wait_twint(void);
void    i2c_init_com(uint8_t addr, uint8_t rw);
uint8_t i2c_get_status(void);
void    i2c_print_status(void);
void    i2c_scan(void);

void    i2c_write(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);
void    i2c_read(void);
