MCU=atmega32u4
USB_PORT=/dev/ttyACM0

INCLUDES=../../../include

.PHONY: clean upload

all: main.hex

%.o: %.c
	avr-gcc -I$(INCLUDES) -Os -DF_CPU=16000000UL -mmcu=$(MCU) -D__AVRKIT_$(MCU)__ -c -o $@ $<

%.elf: %.o
	avr-gcc -mmcu=$(MCU) $< -o $@

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload: main.hex
	stty -F ${USB_PORT} ispeed 1200 ospeed 1200 ; \
	avrdude -F -V -c avr109 -p ${MCU} -P ${USB_PORT} -b 115200 -U flash:w:$<
