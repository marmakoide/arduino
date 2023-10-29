#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


int
main(void) {
	// Toggle OC1A (aka pin B1) when timer 1 equals to OCR1A
	TCCR1A |= (1 << COM1A0);

	// Clear timer on compare match
	TCCR1B |= (1 << WGM12);

	// Set prescaler to 1024
	TCCR1B |= (1 << CS12) | (1 << CS10);

	// Reset timer after 15625 ticks ie. 1 sec on a 16 / 1024 Mhz clock
	OCR1A = 15624;
	
	// Set pin B1 as output
	DDRB |= _BV(DDB1);
	
	// Enter sleep mode, which we will never leave
	sleep_mode();
}
