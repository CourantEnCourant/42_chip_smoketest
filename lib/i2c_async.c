#include "i2c.h"

static volatile t_i2c g_i2c;

void    			i2c_init_async(void)
{
	// set direction to in 0
	DDRC &= ~(_BV(PC4) | _BV(PC5));

	// 22.5.1 pull up resistor
	PORTC |= _BV(PC4) | _BV(PC5);

	// no prescaler update into TWSR default to 0
	// erase other
	TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));

	// 22.5.2 set bit rate (default prescaler to 1)
	TWBR = compute_bit_rate(F_CPU, F_SCL, SCL_PRESCALER);

	// 22.9.2 activate enable TWI on Control reg
	TWCR |= _BV(TWEN);

	// init global struct
	g_i2c.phase = PHASE_I2C_IDLE;
	g_i2c.sync_active = 0;
	g_i2c.result = RES_I2C_IDLE;
	g_i2c.status = 0;
	g_i2c.timeout = 0;
}

static inline void	i2c_internal_send(uint8_t byte)
{
	TWDR = byte;
	// TWIE is very important as it autorizes interrupt
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE);
	// thw we will catch at the end of hte process
}

static inline void	i2c_internal_stop(void)
{
	g_i2c.phase = PHASE_I2C_STOP;
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

static inline bool	i2c_internal_begin(uint8_t addr, uint8_t reg, uint8_t value, uint8_t read)
{
	uint8_t save_sreg = SREG;
	ft_cli();

	if (addr > 127 || g_i2c.phase != PHASE_I2C_IDLE || g_i2c.sync_active || (TWCR & _BV(TWSTO)))
	{
		SREG = save_sreg;
		return (false);
	}

	// update global i2c

	g_i2c.addr = addr;
	g_i2c.reg = reg;
	g_i2c.value = value;
	g_i2c.is_read = read;
	g_i2c.status = 0;
	g_i2c.timeout = 0;
	g_i2c.result = RES_I2C_BUSY;
	g_i2c.phase = PHASE_I2C_START;

	// 
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE);

	// restore sreg  sei;
	SREG = save_sreg;
	return (true);
}

bool				i2c_write_async(uint8_t addr, uint8_t reg, uint8_t value)
{
	return (i2c_internal_begin(addr, reg, value, 0));
}

bool				i2c_read_async(uint8_t addr, uint8_t reg)
{
	return (i2c_internal_begin(addr, reg, 0, 1));
}

/* ISR */

FT_ISR(TWI_vect)
{
	uint8_t status = TWSR & TW_STATUS_MASK;

	g_i2c.status = status;

	switch (status)
	{
	case TW_START:
		if (g_i2c.phase == PHASE_I2C_START)
		{
			g_i2c.phase = PHASE_I2C_ADDR_W;
			i2c_internal_send(g_i2c.addr << 1 | WRITE);
			return ;
		}
		break;
	case TW_REP_START:
		if (g_i2c.phase == PHASE_I2C_RESTART)
		{
			g_i2c.phase = PHASE_I2C_ADDR_R;
			i2c_internal_send(g_i2c.addr << 1 | READ);
			return ;
		}
		break;
	case TW_MT_SLA_ACK:
		if (g_i2c.phase == PHASE_I2C_ADDR_W)
		{
			g_i2c.phase = PHASE_I2C_REGISTER;
			i2c_internal_send(g_i2c.reg);
			return ;
		}
		break;
	case TW_MT_DATA_ACK:
		if (g_i2c.phase == PHASE_I2C_REGISTER)
		{
			if (g_i2c.is_read)
			{
				g_i2c.phase = PHASE_I2C_RESTART;
				TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE); // inmportant enable interrupt
			}
			else
			{
				g_i2c.phase = PHASE_I2C_WRITE_BYTE;
				i2c_internal_send(g_i2c.value);
			}
			return ;
		}
		if (g_i2c.phase == PHASE_I2C_WRITE_BYTE)
		{
			i2c_internal_stop();
			return ;
		}
		break;
	case TW_MT_ARB_LOST:
		g_i2c.result = RES_I2C_ERROR;
		g_i2c.phase = PHASE_I2C_IDLE;
		TWCR = _BV(TWINT) | _BV(TWEN);
		return ;
	default:
		break;
	}

	// erorr state no match
	g_i2c.result = RES_I2C_ERROR;
	i2c_internal_stop();
}

void				i2c_routine_async(void)
{
	uint8_t saved_sreg = SREG;
	ft_cli();
	if (g_i2c.phase == PHASE_I2C_STOP && !(TWCR & _BV(TWSTO)))
	{
		g_i2c.phase = PHASE_I2C_IDLE;
		if (g_i2c.result == RES_I2C_BUSY)
			g_i2c.result = RES_I2C_OK;
	}
	// restore sei()
	SREG = saved_sreg;
}

void				i2c_tick(void)
{
	uint8_t saved_sreg = SREG;
	ft_cli();

	if (g_i2c.phase != PHASE_I2C_IDLE)
	{
		(g_i2c.timeout)++; // inc timout

		if (g_i2c.timeout >= 25) // 25 * 2 ms  = 50 ms max for a transfer
		{
			g_i2c.result = RES_I2C_ERROR;
			/*
				case error TIMEOUT stop evryting, might need a 
				maunal rest but at least we stop the FSM
			*/
			TWCR = 0;
			TWCR = _BV(TWEN);
			g_i2c.phase = PHASE_I2C_IDLE;
			g_i2c.status = 0xFF; // defined as timout;
		}
	}
	else
		g_i2c.timeout = 0;

	// restore sei()
	SREG = saved_sreg;
}

/* UTILS */

bool				i2c_is_busy_async(void)
{
	uint8_t saved_sreg = SREG;
	bool	busy;

	ft_cli();

	busy = g_i2c.phase != PHASE_I2C_IDLE || g_i2c.sync_active || (TWCR & _BV(TWSTO));

	SREG = saved_sreg;
	return (busy);
}

e_i2c_result		i2c_result_async(void)
{
	return (g_i2c.result);
}

uint8_t				i2c_status_async(void)
{
	return (g_i2c.status);
}
uint8_t				i2c_data_async(void)
{
	return (g_i2c.data);
}
