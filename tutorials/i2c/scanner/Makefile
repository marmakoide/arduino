MCU=atmega328p
SERIAL_PORT=/dev/ttyUSB0


.PHONY: clean upload

all: main.hex

%.o: %.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=$(MCU) -c -o $@ $<

%.elf: %.o
	avr-gcc -mmcu=$(MCU) $< -o $@

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload: main.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P ${SERIAL_PORT} -b 57600 -U flash:w:$<
