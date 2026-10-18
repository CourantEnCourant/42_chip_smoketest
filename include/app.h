#ifndef APP_H
# define APP_H

# include <stdint.h>

# define APP_MODE_COUNT 11

void app_init(void);
void app_tick_2ms(void);
void app_service(void);

void app_next_mode(void);
void app_prev_mode(void);

uint8_t app_get_mode(void);

#endif