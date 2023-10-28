#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


int
main(void) {
	// Toggle OC1A (aka pin B1) when equals to OCR1A
	TCCR1A |= (1 << COM1A0);

	// Clear timer on compare match
	TCCR1A |= (1 << WGM12);

	// Set prescaler to 1024
	TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);

	// Reset timer after 15625 ticks
	OCR1A = 15624;
	
	// Set pin B1 as output
	DDRB |= _BV(DDB1);
	
	// Main loop
	while(1)
		sleep_mode();
}
