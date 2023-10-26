MCU=atmega328p
USB_PORT=/dev/ttyUSB0


.PHONY: clean upload-polling upload-interrupt

all: main-polling.hex main-interrupt.hex

%.o: %.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=$(MCU) -c -o $@ $<

%.elf: %.o
	avr-gcc -mmcu=$(MCU) $< -o $@

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

upload-polling: main-polling.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P ${USB_PORT} -b 115200 -U flash:w:$<

upload-interrupt: main-interrupt.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P ${USB_PORT} -b 115200 -U flash:w:$<
