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

From the Arduino's Uno schematics, we can see that

1. the on-board led is controlled by the pin 13
1. pin 13 is controlled by port *B*, bit 5

Now, let's peek into ATmega328P reference manual, *"I/O Ports"* section.

On the ATmega328P, there's port *B*, *C* and *D*. They are 8 bits ports, 1 bit 
corresponding to 1 digital pin on the Arduino UNO board.

1. Arduino UNO's pins 0 to 7 correponds to port *D*, bits 0 to 7
1. Arduino UNO's pins 8 to 13 corresponds to port *B*, bits 0 to 5
1. Arduino UNO's pins A0 to A5 correponds to port *C*, bits 0 to 5 

Each port is associated to 3 8 bits registers called *DDRx*, *PORTx* and 
*PINx*, with *x* being either *B*, *C*, or *D*. 

1. *DDR* stands for *Data Direction Register*. Setting off or on bit *b* 
of port *x* will set it for read or write operations on port *x*, pin *b*.
1. *PORT* is used to set the output of pins.
1. *PIN* stands for *Port Input Pins*. It's a read only register. Bit *b*
will contain the state of pin *b*

How about port A ? There is no port A. Nobody speaks of port A. Do not ever
mention port A again.

Now, after that peek into the reference manual, the codes makes sense

1. We set to write operations bit 5 of port *B* using the *DDRB* register, as
it drives the on-board led.
1. We use the *PORTB* register to set the pin 5 of port *B*, thus switching
on and off the on-board led. 

### Bit mask computation

*libavr* provides a macro, `_BV(bit_index)`, to compute a 8 bits mask, 
equivalent to `1 << bit_index`, and guaranted to be computed at compilation 
time. Setting bits with a statement such as `DDRB |= _BV(DDB5)` typically 
compile to a single `sbi` CPU instruction.

### Delay implementation

The delay to switch the led on and off is implemented with a busy loop : the
`_delay_ms()` function from *libavr*, in `<util/delay.h>`. This is fine for a 
quick demo, but the usually better way to implement a delay would be using 
timers and interrupts.
