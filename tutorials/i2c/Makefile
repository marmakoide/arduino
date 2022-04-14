SERIAL_PORT=/dev/ttyUSB0


.PHONY: clean upload

all: scanner.hex ssd1306.hex

bitmap-array.c: bitmap.png
	python bitmap-to-code.py $< > $@

%.o: %.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o $@ $<

scanner.elf: scanner.o
	avr-gcc -mmcu=atmega328p $< -o $@

ssd1306.elf: ssd1306.o bitmap-array.o
	avr-gcc -mmcu=atmega328p $< -o $@

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex bitmap-array.c

upload-scanner: scanner.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P ${SERIAL_PORT} -b 115200 -U flash:w:$<

upload-ssd1306: ssd1306.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P ${SERIAL_PORT} -b 115200 -U flash:w:$<
