# led-blinker

This is the Arduino's Hello World : blinking the on-board led. It shows how 
to set a pin on and off.

 * Compile with the following command : `make`
 * Upload to the Arduino with the following command : `make upload`
 * Clean-up with the following command : `make clean`

 You might have to modify the USB device associated to the Arduino UNO when 
 plugged on the USB port. Check the Makefile to do so.


## Notes

### Pins, ports and bits

From the Arduino's Uno schematics, we can see the on-board led is controlled
by the pin 13. Pin 13 is controlled by port B, bit 5.

### Bit mask computation

__libavr__ provides a macro, `_BV(bit_index)`, to compute a 8 bits mask, 
equivalent to `1 << bit_index`, and guaranted to be computed at compilation 
time. Setting bits with a statement such as `DDRB |= _BV(DDB5)` typically 
compile to a single `sbi` CPU instruction.

### Delay implementation

The delay to switch the led on and off is implemented with a busy loop : the
`_delay_ms()` function from __libavr__, in `<util/delay.h>`. This is fine for a 
quick demo, but the usually better way to implement a delay would be using 
timers and interrupts.
