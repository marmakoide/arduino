#include <avr/io.h>


void
setup_adc(uint8_t channel)  {
	// Set ADC clock to 16Mhz/128 = 125 kHz
	// One sampling will take 13 ADC cycles, thus 104 usec
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	// Voltage reference from Avcc (5v)
	ADMUX |= _BV(REFS0);

	// Set the channel to read
	ADMUX &= 0xe0;
	ADMUX |= channel;

	// Switch ADC on
	ADCSRA |= _BV(ADEN);
}


int
main(void) {
	// Initialize ADC to use channel 0
	setup_adc(0);
	
	// Set pin 5 of PORT B for write operations
	DDRB |= _BV(DDB5);

	// Set pin 5 of PORT B low to turn led off
	PORTB &= ~_BV(PORTB5);
	
 	// Main loop
	while(1) {
		// Initiate an ADC read
		ADCSRA |= _BV(ADSC);
		
		// Wait for the ADC read to be completed
		while((ADCSRA & _BV(ADSC)) == 1);
		
		// Read the ADC result
		if (ADCW > 682)
			// Set pin 5 of PORT B high to turn led on
			PORTB |= _BV(PORTB5);
		else
			// Set pin 5 of PORT B low to turn led off
			PORTB &= ~_BV(PORTB5);
	}
}
