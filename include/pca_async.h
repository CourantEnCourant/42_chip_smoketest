#ifndef PCA_ASYNC_H
# define PCA_ASYNC_H

# include <stdint.h>
# include <stdbool.h>

# include <avr/io.h>

# include "i2c.h"
# include "pca.h"


typedef enum	enum_pca_init_state
{
	PCA_BOOT,
	PCA_READY,
	PCA_BOOT_ERROR
}				e_pca_init_state;

typedef struct	s_pca_boot_write
{
	uint8_t reg;
	uint8_t value;
}				t_pca_boot_write;

typedef struct	s_pca
{
	e_pca_init_state	state;
	uint8_t				step;
	bool				waiting;
	uint8_t				leds;
	bool				sw3_pressed;
	uint8_t				error_status;
}				t_pca;

# define PCA_LED_MASK (PCA01_D11 | PCA02_D10 | PCA03_D9)
# define PCA_CATHODE_MASK (PCA04_CA1 | PCA05_CA2 | PCA06_CA3 | PCA07_CA4)


// async layder for access PCA9555
// logical convention as polarity of PORT 0 adn PORT 1 
// are not the same we will preserve our stdanrd
// our approch on avr standra low high activated 

/* uasge

	i2c_init_async();
	pca_init_async();

	sei();
	while (1)
	{
		i2c_service();
		pca_service();
	}
*/

void pca_init_async(void);
void pca_routine_async(void);
bool pca_ready_async(void);
bool pca_failed_async(void);

uint8_t pca_error_status_async(void);

void	pca_leds_set_async(uint8_t led_mask);
void	pca_single_led_set_async(uint8_t led_bit, bool on);

// start transaction 
bool	pca_write_cathodes_async(uint8_t cathode_mask);
bool	pca_write_segments_async(uint8_t segments);

// state of last transaction started
bool	pca_busy_async(void);
bool	pca_write_ok_async(void);
bool	pca_write_error_async(void);

// button mgmt display keep arbitration
bool	pca_button_start_async(void);
bool	pca_button_capture_async(void);
bool	pca_sw3_pressed_async(void);

#endif