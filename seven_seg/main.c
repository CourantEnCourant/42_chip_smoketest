#include "firmware.h"

// no mut
static uint8_t display_buffer[4] = { SEG_NONE , SEG_NONE , SEG_NONE , SEG_NONE }; // { SEG_ONE, SEG_TWO , SEG_THREE , SEG_FOUR };

// mut
static const uint8_t slots[4] = { PCA04_CA1, PCA05_CA2 , PCA06_CA3 , PCA07_CA4 };

void display_refresh(void)
{
	static uint8_t slot = 0;
	static uint8_t prev = 3;

	// erase old digit
	pca_API_clear_slot(slots[prev]);
	// prpe segments
	pca_API_select_segments(display_buffer[slot]);
	// start cathode
	pca_API_select_cathode(slots[slot]);

	// update  metadata
	prev = slot;
	slot = (slot + 1) & 0b00000011;
}

void global_init(void)
{
	uart_init(UBRR);
	i2c_init();

	_delay_ms(40); 

	pca_API_screen_init();
	init_timer1_int_2_ms();

	sei();
}

static uint8_t	lut_digit[] = { SEG_ZERO, SEG_ONE , SEG_TWO , SEG_THREE , SEG_FOUR , SEG_FIVE , SEG_SIX , SEG_SEVEN , SEG_EIGHT , SEG_NINE , SEG_NONE};

void update_display_buffer_unary(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3)
{
	cli();
	display_buffer[0] = lut_digit[s0];
	display_buffer[1] = lut_digit[s1];
	display_buffer[2] = lut_digit[s2];
	display_buffer[3] = lut_digit[s3];
	sei();
}

void update_display_buffer(uint8_t * arr)
{
	cli();
	display_buffer[0] = lut_digit[arr[0]];
	display_buffer[1] = lut_digit[arr[1]];
	display_buffer[2] = lut_digit[arr[2]];
	display_buffer[3] = lut_digit[arr[3]];
	sei();
}

void update_display(uint16_t value)
{
	uint8_t out[4]; //  = { SEG_NONE , SEG_NONE , SEG_NONE , SEG_NONE };

	out[0] = value / 1000;
	out[1] = value / 100 % 10;
	out[2] = value / 10 % 10;
	out[3] = value % 10;

	for (uint8_t i = 0; out[i] == 0 && i < 4; i++)
		out[i] = 10;

	update_display_buffer(out);
}

volatile uint16_t n_cyles = 0;


int main(void)
{
	uint16_t count = 0;
    global_init();

    while (1)
    {
        if (g_refresh)
        {
			n_cyles++;
            g_refresh = 0;
            display_refresh();
        }
		if (n_cyles == 10)
		{
			n_cyles = 0;
			count++;
			update_display(count);
			// update_display_value(9, 8, 7, 6);
		}
    }

    return (0);
}

// int main(void)
// {
//     global_init();

//     while (1)
//     {
// 		if (g_num)
// 		{
// 			pca_API_clear_slot(PCA06_CA3);
// 			pca_API_select_segments(SEG_TWO);
// 			pca_API_select_cathode(PCA07_CA4);
// 		}
// 		else
// 		{
// 			pca_API_clear_slot(PCA07_CA4);
// 			pca_API_select_segments(SEG_FOUR);
// 			pca_API_select_cathode(PCA06_CA3);
// 		}
		
//     }
// }
