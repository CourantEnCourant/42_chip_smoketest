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
	enum e_btn_state {
		PRESSED = 0,
		LIFTED = 1,
	} state;
	t_btn_handler press_handler;
	void*         press_arg;
	t_btn_handler lift_handler;
	void*         lift_arg;
} t_btn;

void init_btn(t_btn*            btn,
              volatile uint8_t* port_ptr,
              uint8_t           pin,
              t_btn_handler     press_handler,
              void*             handler_arg,
              t_btn_handler     lift_handler,
              void*             lift_arg);

void btn_poll_accessible(t_btn* btn);
void btn_poll(t_btn* btn, uint8_t pin_state);

