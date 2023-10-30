#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


ISR(TIMER1_COMPA_vect) {
	// Togge pin B5
	PORTB ^= _BV(PORTB5);
}


int
main(void) {
	// Clear timer on compare match
	TCCR1B |= _BV(WGM12);

	// Set prescaler to 1024
	TCCR1B |= _BV(CS12) | _BV(CS10);

	// Reset timer after 15625 ticks ie. 1 sec on a 16 / 1024 Mhz clock
	OCR1A = 15624;

	// Trigger TIMER1_COMPA interruption
	TIMSK1 |= _BV(OCIE1A);
	
	// Set pin B5 as output
	DDRB |= _BV(DDB5);

	// Enable interurptions
	sei();
	
	// Main loop
	while(1)
		sleep_mode();
}
