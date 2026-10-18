#ifndef MODES_H
# define MODES_H

# include <stdint.h>

void modes_enter(uint8_t mode);
void modes_tick_2ms(uint8_t mode);
void modes_service(uint8_t mode);
void modes_leave(uint8_t mode);

#endif