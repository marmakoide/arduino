 #include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include <avrkit/GPIO.h>


ISR(PCINT0_vect) {
	if (gpio_pin_B4__is_high())
	    gpio_pin_B5__set_low();
	else
	    gpio_pin_B5__set_high();
}


int
main(void) {
	// Trigger pin setup
	gpio_pin_B4__set_as_input();
	gpio_pin_B4__set_high();
	gpio_pin_B4__enable_interrupt();

	// Led setup
	gpio_pin_B5__set_as_output();
	gpio_pin_B5__set_low();
	
	// Main loop
	sei();
	while(1)
		sleep_mode();
}
