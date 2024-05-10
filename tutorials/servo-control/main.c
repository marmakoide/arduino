#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


static const uint16_t min_pulse_duration = 899;
static const uint16_t median_pulse_duration = 1499;
static const uint16_t max_pulse_duration = 2099;

static uint8_t state = 0;
static uint8_t button_was_pressed = 0;


ISR(PCINT0_vect) {
	// Button is pressed
	if (bit_is_set(PINB, PINB4)) {
		PORTB &= ~_BV(PORTB5); // Set pin 5 low to turn led off
		
		if (!button_was_pressed) {
			state = (state + 1) % 4;
			
			switch(state) {
				case 1:
					OCR1B = min_pulse_duration;
					break;
				case 3:
					OCR1B = max_pulse_duration;
					break;					
				default:
					OCR1B = median_pulse_duration;
			}
		}
		
		button_was_pressed = 1;
	}
	// Button is released
	else {
		PORTB |= _BV(PORTB5);  // Set pin 5 high to turn led on
		button_was_pressed = 0;
	}
}


static void
setup_pin_change_interruption() {
	// Trigger pin setup
	DDRB &= ~_BV(DDB4);    // Set pin 4 of PORTB for input
	PORTB |= _BV(PORTB4);  // Enable pull-up
	PCICR |= _BV(PCIE0);   // Enable "Pin Change 0" interrupt
	PCMSK0 |= _BV(PCINT4); // PORTB4 is also PCINT4	
}


static void
setup_timer1() {
	// 50 Hz setup, 0.9 msec pulse
	OCR1A = 19999;
	OCR1B = median_pulse_duration;

	// 
	TCCR1A |=  _BV(COM1B1);

	// PWN, phase correct mode
	TCCR1A |= _BV(WGM10) | _BV(WGM11);
	TCCR1B |= _BV(WGM13);
	
	// Set prescaler to 8
	TCCR1B |= _BV(CS11);	
}


int
main(void) {
	setup_timer1();
	setup_pin_change_interruption();
	
	// Set pin B2 as output
	DDRB |= _BV(DDB2);

	// Set pin B5 as output
	DDRB |= _BV(DDB2);
		
	// Main loop
	sei();
	while(1)
		sleep_mode();
}
