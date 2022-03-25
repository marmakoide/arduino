# led-blinker

This is the Arduino's Hello World : blinking the on-board led. It shows how 
to set a pin on and off.

 * Compile with the following command : `make`
 * Upload to the Arduino with the following command : `make upload`
 * Clean-up with the following command : `make clean`

 You might have to modify the USB device associated to the Arduino UNO when 
 plugged on the USB port. Check the Makefile to do so.


## Notes

The delay to switch the led on and off is implemented with a busy loop : the
`_delay_ms()` function from libavr. This is fine for a quick demo, but the 
usually better way to implement a delay would be using timers and interrupts.
