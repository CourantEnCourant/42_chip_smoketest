#pragma once

#include "i2c.h"
#include "uart.h"

#define PCF8563_ADDR 0x51

typedef struct s_time {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint8_t year;
}   t_time;

void get_time(t_time *t);
void set_time(t_time *t);
void print_time(t_time *t);