#include "modes.h"

#include <avr/io.h>
#include <stdbool.h>

#include "adc.h"
#include "app.h"
#include "date.h"
#include "display.h"
#include "i2c.h"
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

/* ADC modes 0..3 */

// reference + channel for each ADC mode
static const uint8_t g_adc_mux[4] = {
	_BV(REFS0),							// 0 RV1  AVCC, ADC0
	_BV(REFS0) | _BV(MUX0),				// 1 LDR  AVCC, ADC1
	_BV(REFS0) | _BV(MUX1),				// 2 NTC  AVCC, ADC2
	_BV(REFS1) | _BV(REFS0) | _BV(MUX3)	// 3 MCU temp 1.1V, ADC8
};

static void mode_adc_enter(uint8_t mode)
{
	ADMUX = g_adc_mux[mode];
}

static void mode_adc_tick(uint8_t mode)
{
	if (mode == 3)
		display_set_number((get_adc() - T_OS) / K);
	else
		display_set_number(get_adc());
}

/* RTC modes 8..10, PCF8563 read through the async i2c */

#define RTC_POLL_TICKS	50u	// 50 * 2ms = 100ms

#define PCF_MINUTES	0x03
#define PCF_HOURS	0x04
#define PCF_DAYS	0x05
#define PCF_MONTHS	0x07
#define PCF_YEARS	0x08

typedef struct s_rtc_view
{
	uint8_t count;		// registers to read, right aligned on the display
	uint8_t reg[2];
	uint8_t mask[2];	// drop the non BCD flag bits
}				t_rtc_view;

static const t_rtc_view g_rtc_views[3] = {
	{2, {PCF_HOURS, PCF_MINUTES}, {0x3F, 0x7F}},	// 8  hh mm
	{2, {PCF_DAYS, PCF_MONTHS}, {0x3F, 0x1F}},		// 9  dd mm
	{1, {PCF_YEARS}, {0xFF}}						// 10 20 yy
};

static struct
{
	uint8_t bcd[2];
	uint8_t idx;
	uint8_t elapsed;
	bool	due;
	bool	waiting;
}	g_rtc;

static void rtc_show(void)
{
	const uint8_t digits[4] = {
		g_rtc.bcd[0] >> 4, g_rtc.bcd[0] & 0x0F,
		g_rtc.bcd[1] >> 4, g_rtc.bcd[1] & 0x0F
	};

	display_set_digits(digits);
}

static void mode_rtc_enter(uint8_t mode)
{
	(void)mode;

	// a read still in flight from the previous mode is dropped
	g_rtc.waiting = false;
	g_rtc.idx = 0;
	g_rtc.elapsed = 0;
	g_rtc.due = true;
	// only shown by mode 10, 8 and 9 overwrite it
	g_rtc.bcd[0] = 0x20;

	display_clear();
}

static void mode_rtc_tick(uint8_t mode)
{
	(void)mode;

	if (++g_rtc.elapsed >= RTC_POLL_TICKS)
	{
		g_rtc.elapsed = 0;
		g_rtc.due = true;
	}
}

static void mode_rtc_service(uint8_t mode)
{
	const t_rtc_view *view = &g_rtc_views[mode - 8];

	if (g_rtc.waiting)
	{
		if (i2c_is_busy_async())
			return;

		g_rtc.waiting = false;

		// rtc missing or bus error, retry at next poll
		if (i2c_result_async() != RES_I2C_OK)
		{
			g_rtc.idx = 0;
			g_rtc.due = false;
			return;
		}

		g_rtc.bcd[2 - view->count + g_rtc.idx] = i2c_data_async() & view->mask[g_rtc.idx];

		if (++g_rtc.idx == view->count)
		{
			g_rtc.idx = 0;
			g_rtc.due = false;
			rtc_show();
		}
		return;
	}

	// only take the bus between two display frames
	if (!g_rtc.due || !display_bus_free())
		return;

	if (i2c_read_async(PCF8563_ADDR, view->reg[g_rtc.idx]))
		g_rtc.waiting = true;
}

/* to update
	enter   : once when entering the mode
	tick    : every 2ms while the mode is active
	service : every main loop iteration
	leave   : once before leaving the mode
*/
static const t_mode_ops g_modes[APP_MODE_COUNT] = {
	{mode_adc_enter,     mode_adc_tick, 0, 0},                // 0 RV1
	{mode_adc_enter,     mode_adc_tick, 0, 0},                // 1 LDR
	{mode_adc_enter,     mode_adc_tick, 0, 0},                // 2 NTC
	{mode_adc_enter,     mode_adc_tick, 0, 0},                // 3 MCU temp
	{mode_4_enter,       0, 0, 0},                            // 4 RGB
	{mode_pending_enter, 0, 0, 0},                            // 5 Celsius
	{mode_pending_enter, 0, 0, 0},                            // 6 Fahrenheit
	{mode_pending_enter, 0, 0, 0},                            // 7 Humidity
	{mode_rtc_enter,     mode_rtc_tick, mode_rtc_service, 0}, // 8 RTC time
	{mode_rtc_enter,     mode_rtc_tick, mode_rtc_service, 0}, // 9 Day/month
	{mode_rtc_enter,     mode_rtc_tick, mode_rtc_service, 0}  // 10 Year
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
