#pragma once

#include "button.h"

#include <avr/io.h>

#define LED_PORT             PORTB
#define LED_PIN              PINB
#define LED_DDR              DDRB

#define D1                   PB0
#define D2                   PB1
#define D3                   PB2
#define D4                   PB4

#define LED_RGB_PORT         PORTD
#define LED_RGB_PIN          PIND
#define LED_RGB_DDR          DDRD

#define D5R                  PD5
#define D5G                  PD6
#define D5B                  PD3

#define SWITCH_PORT          PORTD
#define SWITCH_PIN           PIND
#define SWITCH_DDR           DDRD

#define SW1                  PD2
#define SW2                  PD4

#define set_bit(reg, bit)    ((reg) |= (uint8_t)(1U << (bit)))
#define clear_bit(reg, bit)  ((reg) &= (uint8_t)~(1U << (bit)))
#define toggle_bit(reg, bit) ((reg) ^= (uint8_t)(1U << (bit)))
#define read_pin(reg, bit)   ((uint8_t)(((reg) >> (bit)) & 1U))
