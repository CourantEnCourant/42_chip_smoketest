#include "app.h"

#include "display.h"
#include "led.h"
#include "modes.h"
#include "pca_async.h"

#define BOOT_ON_TICKS	1500u
#define BOOT_OFF_TICKS	500u

typedef enum	enum_app_phase
{
	APP_WAIT_PCA,
	APP_BOOT_ON,
	APP_BOOT_OFF,
	APP_RUNNING
}				e_app_phase;

static e_app_phase g_phase;
static uint16_t g_elapsed;
static uint8_t g_mode;

void app_init(void)
{
	const uint8_t all_segments[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	g_phase = APP_WAIT_PCA;
	g_elapsed = 0;
	g_mode = 0;

	led_all_lights_on();
	display_set_raw_segments(all_segments);
}

uint8_t app_get_mode(void)
{
	return g_mode;
}

static void app_set_mode(uint8_t next)
{
	if (next == g_mode)
		return;

	if (g_phase == APP_RUNNING)
		modes_leave(g_mode);

	g_mode = next;

	// during the init anim we register the buttons
	if (g_phase == APP_RUNNING)
	{
		led_display_mode(g_mode);
		modes_enter(g_mode);
	}
}

void app_next_mode(void)
{
	if (g_mode + 1 < APP_MODE_COUNT)
		app_set_mode(g_mode + 1);
	else
		app_set_mode(0);
}

void app_prev_mode(void)
{
	if (g_mode > 0)
		app_set_mode(g_mode - 1);
	else
		app_set_mode(APP_MODE_COUNT - 1);
}

void app_tick_2ms(void)
{
	if (g_phase == APP_WAIT_PCA)
	{
		if (pca_ready_async())
		{
			g_elapsed = 0;
			g_phase = APP_BOOT_ON;
		}
		return;
	}

	if (g_phase == APP_BOOT_ON)
	{
		if (++g_elapsed >= BOOT_ON_TICKS)
		{
			g_elapsed = 0;
			g_phase = APP_BOOT_OFF;

			led_all_lights_off();
			display_clear();
		}
		return;
	}

	if (g_phase == APP_BOOT_OFF)
	{
		if (++g_elapsed >= BOOT_OFF_TICKS)
		{
			g_phase = APP_RUNNING;

			led_display_mode(g_mode);
			modes_enter(g_mode);
		}
		return;
	}

	modes_tick_2ms(g_mode);
}

void app_service(void)
{
	if (g_phase == APP_RUNNING)
		modes_service(g_mode);
}
