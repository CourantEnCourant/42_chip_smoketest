NAME				= firmware

CC					= avr-gcc
OBJCOPY 			= avr-objcopy
DUDE				= avrdude
RM					= rm -f

F_CPU				= 16000000UL
BAUD_RATE			= 115200UL

CC_FLAGS 			= -g -Os -DF_CPU=$(F_CPU) -DBAUD_RATE=$(BAUD_RATE) -mmcu=atmega328p -MMD -MP
OBJCOPY_FLAGS		= -O ihex -R .eeprom
DUDE_FLAGS			= -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 115200

C_SRCS				= main.c uart.c i2c.c pca.c pca_led_11_10_9.c pca_screen_API.c timer.c

O_SRCS				= $(C_SRCS:.c=.o)
D_SRCS				= $(O_SRCS:.o=.d)

all: hex flash clean

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

$(NAME).bin: $(O_SRCS)
	$(CC) -mmcu=atmega328p -o $@ $(O_SRCS)

$(NAME).hex: $(NAME).bin
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

hex: $(NAME).hex

flash: $(NAME).hex
	$(DUDE) $(DUDE_FLAGS) -U flash:w:$(NAME).hex

clean:
	$(RM) $(O_SRCS) $(D_SRCS) $(NAME).bin $(NAME).hex

re: clean all

-include $(D_SRCS)

.PHONY: all hex flash clean re