#ifndef PCA_H
# define PCA_H

# include <avr/io.h>
# include <util/delay.h>
# include <util/twi.h>
# include <avr/interrupt.h>

# include "uart.h"
# include "i2c.h"

/* pca_screen_API.c */
uint8_t	pca_API_segment(uint8_t digit);
bool	pca_API_screen_clear(void);
bool	pca_API_screen_init(void);
bool	pca_API_select_segments(uint8_t segments);
bool	pca_API_select_cathode(uint8_t cathode);
bool	pca_API_clear_slot(uint8_t cathode);

/* pca.c */
bool pca_write(uint8_t reg, uint8_t value);
bool pca_read(uint8_t reg, uint8_t *value);
bool pca_clear_bits(uint8_t reg, uint8_t bits);
bool pca_set_bits(uint8_t reg, uint8_t bits);
bool pca_start_sequence(uint8_t rw);

/* pca_led_11_10_9.c */
void pca_led_11_10_9_init(void);
void pca_led_11_10_9_update(uint8_t value_3b);

// doc page 19

// i2c addr (table 8-2)
// A2:A0 == [ 1 1 1 ] == 0x39 ??? TO VERIFY
# define PCA_ADDR				0x20

// Command Bytes for Internal Port Access (aka IPA lol)

// input
# define PCA_IN0	0x00	// read only
# define PCA_IN1	0x01	// read only

// output
# define PCA_OUT0	0x02
# define PCA_OUT1	0x03

// polarity
# define PCA_POL0	0x04
# define PCA_POL1	0x05

// conf direction (set to 1 initially) (aka DDR)
# define PCA_CONF0	0x06
# define PCA_CONF1	0x07

// for clean/clear access

// P0
# define PCA00_SW3		_BV(0)
# define PCA01_D11		_BV(1)
# define PCA02_D10		_BV(2)
# define PCA03_D9		_BV(3)
# define PCA04_CA1		_BV(4)
# define PCA05_CA2		_BV(5)
# define PCA06_CA3		_BV(6)
# define PCA07_CA4		_BV(7)

// P1
# define PCA10_A		_BV(0)
# define PCA11_B		_BV(1)
# define PCA12_C		_BV(2)
# define PCA13_D		_BV(3)
# define PCA14_E		_BV(4)
# define PCA15_F		_BV(5)
# define PCA16_G		_BV(6)
# define PCA17_DPX		_BV(7)

// segments

// use to clear all slot
# define SEG_FULL	(PCA10_A | PCA11_B | PCA12_C | PCA13_D | PCA14_E | PCA15_F | PCA16_G)

# define SEG_ZERO	(PCA10_A | PCA11_B | PCA12_C | PCA13_D | PCA14_E | PCA15_F)
# define SEG_ONE	(PCA11_B | PCA12_C)
# define SEG_TWO	(PCA10_A | PCA11_B | PCA16_G | PCA14_E | PCA13_D)
# define SEG_THREE	(PCA10_A | PCA11_B | PCA16_G | PCA12_C | PCA13_D)
# define SEG_FOUR	(PCA15_F | PCA11_B | PCA16_G | PCA12_C)
# define SEG_FIVE	(PCA10_A | PCA15_F | PCA16_G | PCA12_C | PCA13_D)
# define SEG_SIX	(PCA10_A | PCA16_G | PCA15_F | PCA14_E | PCA12_C | PCA13_D)
# define SEG_SEVEN	(PCA10_A | PCA11_B | PCA12_C)
# define SEG_EIGHT	(PCA10_A | PCA11_B | PCA12_C | PCA13_D | PCA14_E | PCA15_F | PCA16_G)
# define SEG_NINE	(PCA10_A | PCA16_G | PCA15_F | PCA11_B | PCA12_C | PCA13_D)
# define SEG_NONE	0


/* API conversion for Same USER experience */
// use AVR high activated for leds and low activated for switch
// write a 1 to high and 0 to low

// to make this transition effective when you write or read to a port, you have to use the specially defined function

# define PCA_INV_PORT0	0xFF

static inline bool pca_write_PORT0(uint8_t value)
{
	return (pca_write(PCA_OUT0, value ^ PCA_INV_PORT0));
}

static inline bool pca_write_PORT1(uint8_t value)
{
	return (pca_write(PCA_OUT1, value));
}

static inline bool pca_read_PORT0(uint8_t *value)
{
	uint8_t raw;

	if (!value || !pca_read(PCA_IN0, &raw))
		return (false);
	
	*value = raw ^ PCA_INV_PORT0;
	return (true);
}

static inline bool pca_read_PORT1(uint8_t *value)
{
	uint8_t raw;

	if (!value || !pca_read(PCA_IN1, &raw))
		return (false);
	
	*value = raw;
	return (true);
}

static inline bool pca_set_bits_PORT0(uint8_t bits)
{
	uint8_t raw;
	uint8_t data;

	if (!pca_read(PCA_OUT0, &raw))
		return (false);

	data = raw ^ PCA_INV_PORT0;

	data |= bits;

	return (pca_write(PCA_OUT0, data ^ PCA_INV_PORT0));
}

static inline bool pca_clear_bits_PORT0(uint8_t bits)
{
	uint8_t raw;
	uint8_t data;

	if (!pca_read(PCA_OUT0, &raw))
		return (false);

	data = raw ^ PCA_INV_PORT0;

	data &= ~bits;

	return (pca_write(PCA_OUT0, data ^ PCA_INV_PORT0));
}

static inline bool pca_set_bits_PORT1(uint8_t bits)
{
	uint8_t data;

	if (!pca_read(PCA_OUT1, &data))
		return (false);

	data |= bits;

	return (pca_write(PCA_OUT1, data));
}

static inline bool pca_clear_bits_PORT1(uint8_t bits)
{
	uint8_t data;

	if (!pca_read(PCA_OUT1, &data))
		return (false);

	data &= ~bits;

	return (pca_write(PCA_OUT1, data));
}

#endif
