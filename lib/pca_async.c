#include "pca_async.h"

static const t_pca_boot_write g_boot[] = { { PCA_OUT0,  0xFF }, { PCA_OUT1,  0x00 }, { PCA_CONF0, PCA00_SW3 }, { PCA_CONF1, 0x00 } };

#define PCA_BOOT_COUNT (sizeof(g_boot) / sizeof(g_boot[0]))

static volatile t_pca g_pca;

void	pca_init_async(void)
{
	// dont start communication
	g_pca.state = PCA_BOOT;
	g_pca.step = 0;
	g_pca.waiting = false;
	g_pca.leds = 0;
	g_pca.sw3_pressed = false;
	g_pca.error_status = 0;
}

void pca_routine_async(void)
{
	if (g_pca.state != PCA_BOOT)
		return ;

	// a transaction have been initiated at last call
	if (g_pca.waiting)
	{
		// check runnign action
		if (i2c_is_busy_async())
			return ;
		// stop happend we can access result

		if (i2c_result_async() != RES_I2C_OK)
		{
			g_pca.error_status = i2c_status_async();
			g_pca.state = PCA_BOOT_ERROR;
			return ;
		}

		g_pca.waiting = false;
		g_pca.step++;

		if (g_pca.step == PCA_BOOT_COUNT)
		{
			g_pca.state = PCA_READY;
			return ;
		}
	}

	// bus is occupied ?
	if (i2c_is_busy_async())
		return ;
	
	// launch writting boot cmds / non blocking
	if (i2c_write_async(PCA_ADDR, g_boot[g_pca.step].reg, g_boot[g_pca.step].value))
		g_pca.waiting = true;
}

bool pca_ready_async(void)
{
	return (g_pca.state == PCA_READY);
}

bool pca_failed_async(void)
{
	return (g_pca.state == PCA_BOOT_ERROR);
}

uint8_t pca_error_status_async(void)
{
	return (g_pca.error_status);
}

void	pca_leds_set_async(uint8_t led_mask)
{
	// only oupdate global struct will be updated if needed
	g_pca.leds = led_mask & PCA_LED_MASK;
}

void	pca_single_led_set_async(uint8_t led_bit, bool on)
{
	led_bit &= PCA_LED_MASK;

	if (on)
		g_pca.leds |= led_bit;
	else
		g_pca.leds &= ~(led_bit);
}

/* DISPLAY MGMT */

bool	pca_write_cathodes_async(uint8_t cathode_mask)
{
	uint8_t logical_port0;

	if (!pca_ready_async() || i2c_is_busy_async())
		return (false);

	// inverted as polarity is different on port 0 and port 1
	logical_port0 = g_pca.leds | (cathode_mask & PCA_CATHODE_MASK);

	return (i2c_write_async(PCA_ADDR, PCA_OUT0, ~(logical_port0)));
}

bool	pca_write_segments_async(uint8_t segments)
{
	if (!pca_ready_async() || i2c_is_busy_async())
		return (false);
	return (i2c_write_async(PCA_ADDR, PCA_OUT1, segments));
}

/* STATE CHECKERS */
bool	pca_busy_async(void)
{
	return (i2c_is_busy_async());
}

bool	pca_write_ok_async(void)
{
	return (!i2c_is_busy_async() && i2c_result_async() == RES_I2C_OK);
}

bool	pca_write_error_async(void)
{
	return (!i2c_is_busy_async() && i2c_result_async() == RES_I2C_ERROR);
}

/* SW3 */

bool	pca_button_start_async(void)
{
	if (!pca_ready_async() || i2c_is_busy_async())
		return (false);
	return (i2c_read_async(PCA_ADDR, PCA_IN0));
}

bool	pca_button_capture_async(void)
{
	if (i2c_is_busy_async() || i2c_result_async() != RES_I2C_OK)
		return (false);

	g_pca.sw3_pressed = ((i2c_data_async() & PCA00_SW3) == 0);
	return (true);
}
bool	pca_sw3_pressed_async(void)
{
	return (g_pca.sw3_pressed);
}