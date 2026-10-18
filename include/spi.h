#pragma once

#include "avr/io.h"
#include "util/delay.h"
#include "avr/interrupt.h"

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

void apa102_stop(void);
void apa102_start(void);
void apa102_color_rgb(uint8_t bright, uint32_t color);
void apa102_transmit_u32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
void spi_transmit(uint8_t data);
void spi_init(void);
