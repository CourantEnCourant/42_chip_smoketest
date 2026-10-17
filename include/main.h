#pragma once

#include <stdint.h>

#ifndef F_CPU
# define F_CPU 16000000UL
#endif

void sw1_press_handler(uint8_t* arg);
void sw1_lift_handler(void* arg);
void sw2_press_handler(uint8_t* arg);
void sw2_lift_handler(void* arg);
void sw3_press_handler(void* arg);
void sw3_lift_handler(void* arg);

void timer1_interrupt_init(void);
