#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


// --- Trigger pin interrupt routine ------------------------------------------

ISR(PCINT0_vect) {
	if (bit_is_set(PINB, PINB4))
		PORTB &= ~_BV(PORTB5); // Set pin 5 low to turn led off
	else
		PORTB |= _BV(PORTB5); // Set pin 5 high to turn led on
}


// --- Main entry point -------------------------------------------------------

int
main(void) {
	// --- trigger pin setup -----------------------------------------------------
	DDRB &= ~_BV(DDB4);    // Set pin 5 of PORTB for input
    PORTB |= _BV(PORTB4);  // Enable pull-up
    PCICR |= _BV(PCIE0);   // Enable "Pin Change 0" interrupt
    PCMSK0 |= _BV(PCINT4); // PORTB4 is also PCINT4

    // --- led setup -------------------------------------------------------------
	DDRB |= _BV(DDB5);     // Set pin 5 of PORTB for output
	PORTB &= ~_BV(PORTB5); // Set pin 5 low to turn led off

	// --- Main loop -------------------------------------------------------------
 	sei();
	while(1)
		sleep_mode();
}
