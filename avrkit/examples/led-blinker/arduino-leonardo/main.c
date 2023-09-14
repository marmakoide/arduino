#include <util/delay.h>

#include <avrkit/GPIO.h>

 
#define BLINK_DELAY_MS 1000

int
main(void) {
	gpio_pin_C7__set_as_output();

	while(1) {
		gpio_pin_C7__set_high();
		_delay_ms(BLINK_DELAY_MS);

		gpio_pin_C7__set_low();
		_delay_ms(BLINK_DELAY_MS);
	}
}
