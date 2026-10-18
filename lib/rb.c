#include "rb.h"

#include <stdint.h>

static uint8_t rb_next(uint8_t i) {
	return i + 1 == RB_SIZE ? 0 : i + 1;
}

char rb_pop_front(t_rb* rb) {
	if (rb->head == rb->tail)
		return '\0';
	char c = rb->buf[rb->head];
	rb->head = rb_next(rb->head);
	return c;
}

void rb_push(t_rb* rb, char c) {
	if (rb_next(rb->tail) == rb->head)
		rb->f(rb_pop_front(rb));
	rb->buf[rb->tail] = c;
	rb->tail = rb_next(rb->tail);
}
