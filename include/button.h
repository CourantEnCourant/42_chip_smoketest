#pragma once

#include <avr/io.h>
#include <stdint.h>

#define BTN_COUNT_THRESHOLD  8

#define is_pressed(sfr, pin) (bit_is_clear(sfr, pin))

typedef void (*t_btn_handler)(void*);
void press_release_event(const uint8_t pin, t_btn_handler, void* handler_arg);

typedef struct s_btn {
	volatile uint8_t* port_ptr;
	uint8_t           pin;
	uint8_t           count;
	uint8_t           state;
	t_btn_handler     handler;
	void*             handler_arg;
} t_btn;

void init_btn(t_btn* btn, volatile uint8_t* port_ptr, uint8_t pin, t_btn_handler handler, void* handler_arg);
void btn_poll(t_btn *btn);
