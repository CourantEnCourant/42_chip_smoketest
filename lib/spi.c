#include "spi.h"

void spi_init(void)
{
	// set MOSI and SCK as output (1)
	DDRB = _BV(PB3) | _BV(PB5)  | _BV(PB2);

	// enable SPI, as Master, then set clock at prescaler fck/16
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
	// intended overide = impose clear control reg

	apa102_start();
	apa102_color_rgb(B_ZERO, RED);
	apa102_color_rgb(B_ZERO, GREEN);
	apa102_color_rgb(B_ZERO, BLUE);
	apa102_stop();
}

void spi_transmit(uint8_t data)
{
	// data reg
	SPDR = data;
	
	// wait for end of transmission
	while (!(SPSR & _BV(SPIF)))
		;
}

void apa102_transmit_u32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	spi_transmit(b0);
	spi_transmit(b1);
	spi_transmit(b2);
	spi_transmit(b3);
}

void apa102_color_rgb(uint8_t bright, uint32_t color)
{
	spi_transmit(bright);
	// transmit order inverted
	spi_transmit(color);		// B
	spi_transmit(color >> 8);	// G
	spi_transmit(color >> 16);	// R
}

void apa102_start(void)
{
	apa102_transmit_u32(ZERO, ZERO, ZERO, ZERO);
}

void apa102_stop(void)
{
	apa102_transmit_u32(ONE, ONE, ONE, ONE);
}