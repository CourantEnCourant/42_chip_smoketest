#include "date.h"

void get_time(t_time *t) {
    i2c_start();
    i2c_write(PCF8563_ADDR << 1 | WRITE);   // write mode
    i2c_write(0x02);
    i2c_start();
    i2c_write(PCF8563_ADDR << 1 | READ);
    t->sec = i2c_read_byte(0);
    t->min = i2c_read_byte(0);
    t->hour = i2c_read_byte(0);
    t->day = i2c_read_byte(0);
    t->weekday = i2c_read_byte(0);
    t->month = i2c_read_byte(0);
    t->year = i2c_read_byte(1);
    i2c_stop();

    // convert BCD
    t->sec = ((t->sec >> 4) & 0x07) * 10 + (t->sec & 0x0F);
    t->min = ((t->min >> 4) & 0x07) * 10 + (t->min & 0x0F);
    t->hour = ((t->hour >> 4) & 0x03) * 10 + (t->hour & 0x0F);
    t->day = ((t->day >> 4) & 0x03) * 10 + (t->day & 0x0F);
    t->weekday = (t->weekday & 0x07);
    t->month = ((t->month >> 4) & 0x01) * 10 + (t->month & 0x0F);
    t->year = ((t->year >> 4) & 0x0F) * 10 + (t->year & 0x0F);
}

static uint8_t to_BCD(uint8_t n) {
    return ((n / 10) << 4) | (n % 10);
}

void set_time(t_time *t) {
    i2c_start();
    i2c_write(PCF8563_ADDR << 1 | WRITE);   // write mode
    i2c_write(0x02);
    i2c_write(to_BCD(t->sec));
    i2c_write(to_BCD(t->min));
    i2c_write(to_BCD(t->hour));
    i2c_write(to_BCD(t->day));
    i2c_write(to_BCD(t->weekday));
    i2c_write(to_BCD(t->month));
    i2c_write(to_BCD(t->year));
    i2c_stop();
}

void print_time(t_time *t) {
    print_2digit(t->day);
    uart_tx('/');
    print_2digit(t->month);
    uart_tx('/');
    print_2digit(t->year);
    uart_tx(' ');
    print_2digit(t->hour);
    uart_tx(':');
    print_2digit(t->min);
    uart_tx(':');
    print_2digit(t->sec);
}