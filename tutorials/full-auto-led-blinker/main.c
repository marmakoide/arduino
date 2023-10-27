#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


int
main(void) {
	// Reset timer after 15624 ticks, 16 MHz / (1024 * 15625) = 1Hz
	OCR1A = 15624;

	//
	TCCR1A |= (1 << COM1A0);
	TCCR1A |= (1 << WGM12);

	// Set prescaler to 1024
	TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);
	
	// Set pin B1 as output
	DDRB |= _BV(DDB1);
	
	// Main loop
	while(1)
		sleep_mode();
}
