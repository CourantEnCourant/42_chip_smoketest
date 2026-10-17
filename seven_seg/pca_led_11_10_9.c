#include "pca.h"

/* tested in ex01 m09 */

void pca_led_11_10_9_init(void)
{
	// clear bit to set the IO to IN (0 here instead of 1 for AVR)
	pca_clear_bits(PCA_CONF0, PCA03_D9 | PCA02_D10 | PCA01_D11);
	// shut down leds, as low activated we need to write a 1 to them to succeed
	pca_set_bits(PCA_OUT0, PCA03_D9 | PCA02_D10 | PCA01_D11);
}

void pca_led_11_10_9_update(uint8_t value_3b)
{
	// clear old value
	pca_set_bits(PCA_OUT0, PCA03_D9 | PCA02_D10 | PCA01_D11);
	
	// set new value
	pca_clear_bits(PCA_OUT0, (value_3b & 0b00000111) << 1);
}
