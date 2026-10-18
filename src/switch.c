#include "main.h"

#include "app.h"
#include "pca.h"
#include "pca_async.h"

void sw1_press_handler(void *arg)
{
	(void)arg;

	pca_single_led_set_async(PCA03_D9, true);
	app_next_mode();
}

void sw1_lift_handler(void *arg)
{
	(void)arg;

	pca_single_led_set_async(PCA03_D9, false);
}

void sw2_press_handler(void *arg)
{
	(void)arg;

	pca_single_led_set_async(PCA02_D10, true);
	app_prev_mode();
}

void sw2_lift_handler(void *arg)
{
	(void)arg;

	pca_single_led_set_async(PCA02_D10, false);
}

void sw3_press_handler(void *arg)
{
	(void)arg;

	pca_single_led_set_async(PCA01_D11, true);
}

void sw3_lift_handler(void *arg)
{
	(void)arg;

	pca_single_led_set_async(PCA01_D11, false);
}
