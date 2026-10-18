#include "modes.h"

#include "app.h"
#include "display.h"
#include "pca.h"

typedef void (*t_mode_callback)(uint8_t);

typedef struct s_mode_ops
{
	t_mode_callback enter;
	t_mode_callback tick;
	t_mode_callback service;
	t_mode_callback leave;
}				t_mode_ops;

static void mode_pending_enter(uint8_t mode)
{
	display_set_number(mode);
}

static void mode_4_enter(uint8_t mode)
{
	(void)mode;
	
	const uint8_t segments[4] = {PCA16_G,SEG_FOUR,SEG_TWO,PCA16_G};

	display_set_raw_segments(segments);
}

/* to update
	enter   : once when entering the mode
	tick    : every 2ms while the mode is active
	service : every main loop iteration
	leave   : once before leaving the mode
*/
static const t_mode_ops g_modes[APP_MODE_COUNT] = {
	{mode_pending_enter, 0, 0, 0}, // 0 RV1
	{mode_pending_enter, 0, 0, 0}, // 1 LDR
	{mode_pending_enter, 0, 0, 0}, // 2 NTC
	{mode_pending_enter, 0, 0, 0}, // 3 MCU temp
	{mode_4_enter,        0, 0, 0}, // 4 RGB
	{mode_pending_enter, 0, 0, 0}, // 5 Celsius
	{mode_pending_enter, 0, 0, 0}, // 6 Fahrenheit
	{mode_pending_enter, 0, 0, 0}, // 7 Humidity
	{mode_pending_enter, 0, 0, 0}, // 8 RTC time
	{mode_pending_enter, 0, 0, 0}, // 9 Day/month
	{mode_pending_enter, 0, 0, 0}  // 10 Year
};

void modes_enter(uint8_t mode)
{
	if (mode < APP_MODE_COUNT && g_modes[mode].enter)
		g_modes[mode].enter(mode);
}

void modes_tick_2ms(uint8_t mode)
{
	if (mode < APP_MODE_COUNT && g_modes[mode].tick)
		g_modes[mode].tick(mode);
}

void modes_service(uint8_t mode)
{
	if (mode < APP_MODE_COUNT && g_modes[mode].service)
		g_modes[mode].service(mode);
}

void modes_leave(uint8_t mode)
{
	if (mode < APP_MODE_COUNT && g_modes[mode].leave)
		g_modes[mode].leave(mode);
}
