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
#define ONE		0b11111111

#define B_LOW	0b11100001
#define B_MID	0b11101000
#define B_HIGH	0b11111111

#define B_ZERO	0b11100000

#define RED		(0xF00000)

#define GREEN	(0x00F000)
#define BLUE	(0x0000F0)
#define YELLOW	(0xF0F000)
#define CYAN	(0x00F0F0)
#define MAGENTA	(0xF000F0)
#define WHITE	(0xF0F0F0)

// #define RED		(0x010000)
// #define GREEN	(0x000100)
// #define BLUE	(0x000001)
// #define YELLOW	(0x010100)
// #define CYAN	(0x000101)
// #define MAGENTA	(0x010001)
// #define WHITE	(0x010101)

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

int main(void)
{
	uint32_t colors[7] = { RED , GREEN , BLUE , YELLOW , CYAN , MAGENTA , WHITE };

	spi_init();
	
	while (1)
	{
		for (uint8_t i = 0; i < 7; i++)
		{
			apa102_start();

			apa102_color_rgb(B_LOW, colors[i]);
			apa102_color_rgb(B_ZERO, 0);
			apa102_color_rgb(B_ZERO, 0);

			apa102_stop();

			_delay_ms(1000);
		}
	}

	return (0);
}