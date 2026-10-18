_This project has been created as part of the 42 curriculum by andelalo, rstarce, weizhang._

# 42_chip_smoketest

**Self-test firmware for the 42chips devboard — ATmega328P, 16 MHz**

## Description

### 1. General description

42_chip_smoketest is the firmware for **Rush 01: Final boss** of the 42chips Embedded Pool.
It exercises every peripheral of the devboard and reports the result on the 7-segment display
and the onboard LEDs. The firmware is written in bare-metal C using only `avr/io.h`,
`util/delay.h` and `avr/interrupt.h`.

### 2. Features

- Non-blocking startup sequence: 7-segment display and D1–D4 on 3 s, off 1 s
- 11 display modes (0–10), selected with SW1 / SW2
- Current mode shown in binary on D1–D4
- Button feedback: SW1 → D9, SW2 → D10, SW3 → D11
- Analog measurements: RV1 potentiometer, LDR R14, NTC R20, MCU internal temperature sensor
- Ambient temperature (°C / °F) and relative humidity from the AHT20 sensor
- RGB sequence on D5–D8: red → green → blue, 1 s per color
- Real-time clock on PCF8563, battery-backed: time is retained across power cycles
- Date and time set over UART
- Fully interrupt-driven timing: no busy-wait delays in the main loop

### 3. Peripheral usage

| Peripheral | Configuration                                         | Function                                      |
| ---------- | ----------------------------------------------------- | --------------------------------------------- |
| Timer1     | CTC (mode 4), prescaler 64, `OCR1A = 499` → 2 ms tick | System tick, button polling, animation timing |
| ADC        | AVCC ref for ADC0–ADC2, internal 1.1 V ref for ADC8   | RV1, LDR, NTC, internal temperature           |
| USART0     | 115200 baud, 8N1, double speed (`U2X0`)               | Logs, date/time input                         |
| TWI (I2C)  | 100 kHz, prescaler 1                                  | PCA9555, PCF8563, AHT20                       |
| GPIO       | Inputs with pull-up on SW1 / SW2, outputs on D1–D4    | Buttons, mode LEDs                            |

### 4. I2C bus map

| Address | Device                     | Function                               |
| ------- | -------------------------- | -------------------------------------- |
| `0x20`  | PCA9555 16-bit IO expander | 7-segment display, D9–D11, SW3         |
| `0x38`  | AHT20                      | Ambient temperature, relative humidity |
| `0x51`  | PCF8563                    | Real-time clock                        |

### 5. Startup sequence

| Phase | Duration | Output                                             |
| ----- | -------- | -------------------------------------------------- |
| 1     | 0 – 3 s  | All segments of the 7-segment display and D1–D4 on |
| 2     | 3 – 4 s  | All off                                            |
| 3     | from 4 s | Normal operation, mode 0                           |

The sequence is driven by the Timer1 tick. Buttons are serviced during all phases.

### 6. Button functions

| Button | Function               | Indicator            |
| ------ | ---------------------- | -------------------- |
| SW1    | Next mode (10 → 0)     | D9 on while pressed  |
| SW2    | Previous mode (0 → 10) | D10 on while pressed |
| SW3    | —                      | D11 on while pressed |

SW1 and SW2 are polled and debounced in the Timer1 interrupt. SW3 is read through the
PCA9555.

### 7. Operating modes

| Mode | D4–D1  | Source                 | 7-segment output                                | Range         |
| ---- | ------ | ---------------------- | ----------------------------------------------- | ------------- |
| 0    | `0000` | ADC0 — RV1             | Raw value, decimal                              | 0 – 1023      |
| 1    | `0001` | ADC1 — LDR R14         | Raw value, decimal                              | 0 – 1023      |
| 2    | `0010` | ADC2 — NTC R20         | Raw value, decimal                              | 0 – 1023      |
| 3    | `0011` | ADC8 — internal sensor | Temperature, decimal                            | 0 – 1023      |
| 4    | `0100` | —                      | `-42-`; D5–D8 red → green → blue, 1 s per color | —             |
| 5    | `0101` | AHT20                  | Ambient temperature, °C                         | —             |
| 6    | `0110` | AHT20                  | Ambient temperature, °F                         | —             |
| 7    | `0111` | AHT20                  | Relative humidity, %                            | 0 – 100       |
| 8    | `1000` | PCF8563                | Time, 24-hour, `HH:MM`                          | 00:00 – 23:59 |
| 9    | `1001` | PCF8563                | Day and month, `DD.MM`                          | —             |
| 10   | `1010` | PCF8563                | Year                                            | 2000 – 2099   |

### 8. Date and time input

Format, sent over USART0:

```
DD/MM/YYYY HH:MM:SS
```

Example:

```
20/08/2023 22:30:42
```

The input is validated (`parse_time`, `is_valid_date`), converted to BCD and written to the
PCF8563 (`set_time`). The RTC keeps the time while the board is unpowered.

### 9. Source organization

```
src/
  main.c        Initialization, main loop, mode dispatch
  isr.c         Timer1 ISR: system tick, button polling
  switch.c      SW1 / SW2 / SW3 handlers
  timer.c       Timer1 configuration
lib/
  adc.c         ADC driver
  uart.c, rb.c  USART0 driver, ring buffer
  i2c.c         TWI driver (blocking)
  i2c_async.c   TWI driver (interrupt-driven)
  pca*.c        PCA9555 driver: LEDs, 7-segment display, SW3
  display*.c    7-segment multiplexing
  led.c         LED helpers
  date.c        PCF8563 driver, date/time parsing
  print.c       Decimal / hexadecimal output over USART0
  utils.c       String and date helpers
include/        Headers
```

## Instructions

### 1. Requirements

- `avr-gcc`, `avr-libc`, `avr-objcopy`, `avrdude`
- 42chips devboard connected over USB

### 2. Build and flash

| Command      | Effect                           |
| ------------ | -------------------------------- |
| `make`       | Build and flash (default target) |
| `make hex`   | Build `main.hex` only            |
| `make asm`   | Disassemble into `main.s`        |
| `make clean` | Remove build artifacts           |

### 3. Serial port

| Host  | Default port           |
| ----- | ---------------------- |
| macOS | `/dev/cu.usbserial-10` |
| Linux | `/dev/ttyUSB0`         |

Override `PROGRAMMER_ARGS` in the `Makefile` if the board enumerates elsewhere.

### 4. Serial console

115200 baud, 8 data bits, no parity, 1 stop bit.

```sh
screen /dev/cu.usbserial-10 115200     # exit: Ctrl-A, K
```

## Resources

- [ATmega328P datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
- [PCA9555 datasheet (NXP)](https://www.nxp.com/docs/en/data-sheet/PCA9555.pdf)
- [PCF8563 datasheet (NXP)](https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf)
- [AHT20 datasheet (Aosong)](http://www.aosong.com/userfiles/files/media/AHT20%20%E8%8B%B1%E6%96%87%E7%89%88%E8%AF%B4%E6%98%8E%E4%B9%A6%20A0%2020201222.pdf)
- The 42chips Embedded Pool subjects and the devboard schematic

### Use of AI

- **README:** this file was drafted with Claude (Anthropic) from the subject and the source code, then reviewed by the team.
