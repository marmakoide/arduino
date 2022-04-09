SERIAL_PORT=/dev/ttyUSB0


.PHONY: clean upload

all: scanner.hex

%.o: %.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o $@ $<

%.elf: %.o
	avr-gcc -mmcu=atmega328p $< -o $@

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload: scanner.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P ${SERIAL_PORT} -b 115200 -U flash:w:$<
