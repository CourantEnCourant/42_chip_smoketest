#pragma once

#include <stdint.h>

#define RB_SIZE 256

typedef void (*t_rb_handler)(char);

typedef struct s_rb {
	char         buf[RB_SIZE];
	uint8_t      head;
	uint8_t      tail;
	t_rb_handler f;
} t_rb;

char rb_pop_front(t_rb* rb);
void rb_push(t_rb* rb, char c);
