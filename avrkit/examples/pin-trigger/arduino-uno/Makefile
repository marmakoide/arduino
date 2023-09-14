MCU=atmega328p
USB_PORT=/dev/ttyUSB0

INCLUDES=../../../include

.PHONY: clean upload

all: main.hex

%.o: %.c
	avr-gcc -I$(INCLUDES) -Os -DF_CPU=16000000UL -D__AVRKIT_$(MCU)__ -mmcu=$(MCU) -c -o $@ $<

%.elf: %.o
	avr-gcc -mmcu=$(MCU) $< -o $@

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload: main.hex
	avrdude -F -V -c arduino -p ${MCU} -P ${USB_PORT} -b 115200 -U flash:w:$<
