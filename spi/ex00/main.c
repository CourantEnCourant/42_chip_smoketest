#include "avr/io.h"
#include "util/delay.h"
#include "avr/interrupt.h"

/*
	init example page 172

			replace with correct pin value:

				DDR_SPI = DDRB

				DD_SS	= PB2
				DD_MOSI = PB3
				DD_MISO = PB4
				DD_SCK	= PB5

	// MASTER

	void SPI_MasterInit(void)
	{
		// Set MOSI and SCK output, all others input
		DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
		// Enable SPI, Master, set clock rate fck/16
		SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	}
	void SPI_MasterTransmit(char cData)
	{
		// Start transmission
		SPDR = cData;
		// Wait for transmission complete
		while(!(SPSR & (1<<SPIF)))
			;
	}
*/

#define ZERO	0b00000000
#define ONE	0b11111111

#define B_LOW	0b11100001
#define B_MID	0b11101000
#define B_HIGH	0b11111111

#define B_ZERO	0b11100000

void spi_init(void)
{
	// set MOSI and SCK as output (1)
	DDRB = _BV(PB3) | _BV(PB5)  | _BV(PB2);

	// enable SPI, as Master, then set clock at prescaler fck/16
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
	// intended overide = impose clear control reg
}

void spi_transmit(uint8_t data)
{
	// data reg
	SPDR = data;
	
	// wait for end of transmission
	while (!(SPSR & _BV(SPIF)))
		;
}

void apa102_transmit_u32_bgr(uint8_t bright, uint8_t b, uint8_t g, uint8_t r)
{
	spi_transmit(bright);
	spi_transmit(b);
	spi_transmit(g);
	spi_transmit(r);
}

void apa102_transmit_u32_rgb(uint8_t bright, uint8_t r, uint8_t g, uint8_t b)
{
	spi_transmit(bright);
	spi_transmit(b);
	spi_transmit(g);
	spi_transmit(r);
}

void apa102_start(void)
{
	apa102_transmit_u32_rgb(ZERO, ZERO, ZERO, ZERO);
}

void apa102_stop(void)
{
	apa102_transmit_u32_rgb(ONE, ONE, ONE, ONE);
}

int main(void)
{
	spi_init();
	
	apa102_start();

	apa102_transmit_u32_rgb(B_HIGH, 0x01, ZERO, ZERO);
	apa102_transmit_u32_rgb(B_ZERO, ZERO, ZERO, ZERO);
	apa102_transmit_u32_rgb(B_ZERO, ZERO, ZERO, ZERO);

	apa102_stop();

	while (1)
	{

	}

	return (0);
}