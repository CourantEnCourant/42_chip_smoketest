#include "pca.h"

static uint8_t	lut_digit[] = { SEG_ZERO, SEG_ONE , SEG_TWO , SEG_THREE , SEG_FOUR , SEG_FIVE , SEG_SIX , SEG_SEVEN , SEG_EIGHT , SEG_NINE };

uint8_t	pca_API_segment(uint8_t digit)
{
	if (digit > 9) // error
		return (uart_printstr("pca_API_segment error: out of bound in lut\r\n"), lut_digit[0]);
	
	return (lut_digit[digit]);
}

bool pca_API_screen_clear(void)
{
	// clear cathode (set to keep button and leds not affected)
	if(!pca_clear_bits_PORT0(0b11110000))
		return (false);
	// clear anode (intentilnal overwrite)
	if (!pca_write_PORT1(0b00000000))
		return (false);

	return (true);
}

bool pca_API_screen_init(void)
{
	// set cathode as output
	if (!pca_clear_bits(PCA_CONF0, 0b11110000))
		return (false);

	// set anode as outputs
	if (!pca_write(PCA_CONF1, 0b00000000))
		return (false);

	if (!pca_API_screen_clear())
		return (false);

	return (true);
}

bool pca_API_select_segments(uint8_t segments)
{
	return (pca_set_bits_PORT1(segments));
}

bool pca_API_select_cathode(uint8_t cathode)
{
	return (pca_set_bits_PORT0(cathode));
}

bool pca_API_clear_slot(uint8_t cathode)
{
	if (!pca_clear_bits_PORT0(cathode))
		return (false);

	// pca_write_PORT1(~SEG_FULL);
	if (!pca_clear_bits_PORT1(SEG_FULL))
		return (false);
	
	return (true);
}