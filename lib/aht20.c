#include "aht20.h"

void trigger_measure(void) {
    i2c_start();
    i2c_write(AHT20_ADDR << 1 | TW_WRITE);
    i2c_write(0xAC);    // trigger measurement
    i2c_write(0x33);
    i2c_write(0x00);
    i2c_stop();
}

uint8_t get_mesure(uint32_t *sh, uint32_t *st) {
    uint8_t status, ht;

    i2c_start();
    i2c_write(AHT20_ADDR << 1 | TW_READ);
    status = i2c_read_byte(1);
    *sh = i2c_read_byte(1);
    *sh = (*sh << 8) | i2c_read_byte(1);
    ht  = i2c_read_byte(1);
    *sh = (*sh << 4) | (ht >> 4);
    *st = ht & 0x0F;
    *st = (*st << 8) | i2c_read_byte(1);
    *st = (*st << 8) | i2c_read_byte(1);
    i2c_read_byte(0);                 // CRC, NACK = last byte
    i2c_stop();
    return status;
}

int16_t aht20_temp(uint32_t st) {
    return (int16_t)((st * 200UL) >> 20) - 50;
}

int16_t aht20_hum(uint32_t sh) {
    return (int16_t)((sh * 100UL) >> 20);
}

void calibration(void) {
    i2c_start();
    i2c_write(AHT20_ADDR << 1 | TW_WRITE);
    i2c_write(0x71);
    i2c_stop();

    uint8_t status;
    i2c_start();
    i2c_write(AHT20_ADDR << 1 | TW_READ);
    status = i2c_read_byte(0);
    i2c_stop();

    if (!((status >> 3) & 1)) {
        i2c_start();
        i2c_write(AHT20_ADDR << 1 | TW_WRITE);
        i2c_write(0xbe);    // calibration
        i2c_write(0x08);
        i2c_write(0x00);
        i2c_stop();
    }
}