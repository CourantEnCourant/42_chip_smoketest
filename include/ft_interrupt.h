#pragma once

#include <avr/io.h>

#define FT_ISR(VECT) \
	__attribute__((signal, used, externally_visible)) void VECT()

#define ft_sei() (SREG |= 1 << SREG_I)
