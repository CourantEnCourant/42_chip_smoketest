#include "display.h"
#include "pca.h"
#include "pca_async.h"

static const uint8_t g_lut_digits[11] = {
	SEG_ZERO,
	SEG_ONE,
	SEG_TWO,
	SEG_THREE,
	SEG_FOUR,
	SEG_FIVE,
	SEG_SIX,
	SEG_SEVEN,
	SEG_EIGHT,
	SEG_NINE,
	SEG_NONE
};

// cathodes slots
static const uint8_t g_slots[4] = { PCA04_CA1, PCA05_CA2, PCA06_CA3, PCA07_CA4 };

//	display FSM async
typedef enum e_display_state
{
	DISPLAY_IDLE,
	DISPLAY_WAIT_OFF,
	DISPLAY_WAIT_SEGMENTS,
	DISPLAY_WAIT_ON,
	DISPLAY_WAIT_BUTTON,
	DISPLAY_ERROR
} t_display_state;

static uint8_t g_frames[4];
static uint8_t g_slot;
static uint8_t g_segments;
static uint8_t g_refresh_due;
static t_display_state g_state;

bool display_init(void)
{
	g_slot = 0;
	g_segments = SEG_NONE;
	g_refresh_due = 0;
	g_state = DISPLAY_IDLE;

	for (uint8_t i = 0; i < 4; ++i)
		g_frames[i] = SEG_NONE;

	return true;
}

// funciton called every tick
void display_tick(void)
{
	g_refresh_due = 1;
}

bool display_has_error(void)
{
	return g_state == DISPLAY_ERROR;
}

// the state machine !!!
void display_service(void)
{
	// wait init done
	if (!pca_ready_async())
		return;

	switch (g_state)
	{
		case DISPLAY_IDLE:
			// no tick
			if (!g_refresh_due)
				return ;

			// take segment of next number to display
			g_segments = g_frames[g_slot];

			// deactivate cathodes without deactivating leds
			if (pca_write_cathodes_async(0))
			{
				g_refresh_due = 0;
				g_state = DISPLAY_WAIT_OFF;
			}
			return ;

		case DISPLAY_WAIT_BUTTON:
			if (pca_busy_async())
				return;
			g_state = pca_button_capture_async() ? DISPLAY_IDLE : DISPLAY_ERROR;
			return;

		case DISPLAY_WAIT_OFF:
			if (pca_busy_async())
				return;

			if (!pca_write_ok_async())
			{
				g_state = DISPLAY_ERROR;
				return ;
			}

			// diaplay is well deactivated so 
			// we can send the next segment to annodes
			if (pca_write_segments_async(g_segments))
				g_state = DISPLAY_WAIT_SEGMENTS;
			return ;

		case DISPLAY_WAIT_SEGMENTS:
			if (pca_busy_async())
				return ;

			if (!pca_write_ok_async())
			{
				g_state = DISPLAY_ERROR;
				return;
			}

			// now we set the right cathode up 
			// to light up the final digit
			if (pca_write_cathodes_async(g_slots[g_slot]))
				g_state = DISPLAY_WAIT_ON;
			return ;

		case DISPLAY_WAIT_ON:
			if (pca_busy_async())
				return;

			if (!pca_write_ok_async())
			{
				g_state = DISPLAY_ERROR;
				return;
			}

			// modulo 4
			g_slot = (g_slot + 1) & 0x03;
			// set new state idle can take new number in
			if (g_slot == 0 && pca_button_start_async())
				g_state = DISPLAY_WAIT_BUTTON;
			else
				g_state = DISPLAY_IDLE;
			return;

		case DISPLAY_ERROR:
			// we just return
			return;
	}
}

void display_set_digits(const uint8_t digits[4])
{
	if (!digits)
		return;

	for (uint8_t i = 0; i < 4; ++i)
	{
		if (digits[i] <= DISPLAY_BLANK)
			g_frames[i] = g_lut_digits[digits[i]];
		else
			g_frames[i] = SEG_NONE;
	}
}

void display_set_raw_segments(const uint8_t segments[4])
{
	if (!segments)
		return;

	for (uint8_t i = 0; i < 4; ++i)
		g_frames[i] = segments[i];
}

void display_set_number(uint16_t value)
{
	uint8_t digits[4];

	value %= 10000;

	digits[0] = value / 1000;
	digits[1] = (value / 100) % 10;
	digits[2] = (value / 10) % 10;
	digits[3] = value % 10;

	for (uint8_t i = 0; i < 3; ++i)
	{
		if (digits[i] != 0)
			break;

		digits[i] = DISPLAY_BLANK;
	}

	display_set_digits(digits);
}

void display_clear(void)
{
	const uint8_t blank[4] = { DISPLAY_BLANK, DISPLAY_BLANK, DISPLAY_BLANK, DISPLAY_BLANK };

	display_set_digits(blank);
}
