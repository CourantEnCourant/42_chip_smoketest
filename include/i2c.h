#pragma once

#include <stdint.h>

#define F_SCL         100000UL  // 100KHz
#define SCL_PRESCALER 1

#define WRITE         0
#define READ          1

void    i2c_init(void);
void    i2c_start(void);
void    i2c_stop(void);

uint8_t compute_bit_rate(uint32_t f_cpu, uint32_t f_scl, uint8_t prescaler);
void    i2c_wait_twint(void);
void    i2c_init_com(uint8_t addr, uint8_t rw);
uint8_t i2c_get_status(void);
void    i2c_print_status(void);
void    i2c_scan(void);

void    i2c_write(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);
void    i2c_read(void);
