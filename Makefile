# Project specific
MCU				= atmega328p
F_CPU			= 16000000UL
BAUD  			= 115200
PROGRAMMER_TYPE	= arduino
PROGRAMMER_ARGS = -c $(PROGRAMMER_TYPE) -b $(BAUD)  -p $(MCU)

ifeq ($(shell uname -s),Darwin)
	PROGRAMMER_ARGS += -P /dev/cu.usbserial-10
else
	PROGRAMMER_ARGS += -P /dev/ttyUSB0
endif

# Dev toolchain
CC				= avr-gcc
SRC_DIR			= src
INC_DIR			= include
LIB_DIR			= lib
CFLAGS			= -Wall -Wextra -Os -flto -mmcu=$(MCU) -DF_CPU=$(F_CPU) -I$(INC_DIR)
OBJCOPY 		= avr-objcopy
OBJDUMP 		= avr-objdump

HEAD			= $(wildcard $(INC_DIR)/*.h)
SRC				= $(wildcard $(SRC_DIR)/*.c) \
				  $(wildcard $(LIB_DIR)/*.c)
OBJ				= $(SRC:.c=.o)
ELF				= main.elf
HEX				= main.hex
ASM				= main.s

all: flash

hex: $(HEX)

%.o: %.c $(HEAD) Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

flash: $(HEX)
	avrdude $(PROGRAMMER_ARGS) -U flash:w:$<

asm: $(ASM)

$(ASM): $(ELF)
	$(OBJDUMP) -d $< > $@

clean:
	rm -f $(OBJ) $(ELF) $(HEX) $(ASM)

.PHONY: all hex flash asm clean
