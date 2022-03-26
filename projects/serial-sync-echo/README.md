# serial-sync-echo

This program will make the Arduino board echo on the serial port anything that 
it receives in the serial port. 

The program uses synchronous communications, meaning that busy loops are used 
to wait for inputs and outputs. This is generaly simpler to manage, but those 
waiting cycles can't be used to do something else.

  * Compile with the following command : `make`
  * Upload to the Arduino with the following command : `make upload`
  * Clean-up with the following command : `make clean`
  * Launch the serial monitor with the following command : `./serial-com`
  * You can leave the serial monitor session by pressing crtl-A, then crtl-X.
  
You might have to modify the USB device associated to the Arduino UNO when 
plugged on the USB port. Check the Makefile and the [serial-com](serial-com)
script to do so.


## Notes

The program manages a 17 characters internal buffer. If you enter a string 
short enough to not fill that buffer, it will be echoed as expected.

It gets a bit more interesting when a string longer than the internal
buffer is entered. The program will echo the full content of the buffer
before you finished to enter your string.

### UART initialisation and monitoring

*UART* stands for *Universal Asynchronous Receiver Transmitter*, a fairly
universal standard for serial data transmission. On the ATmega328P, there's
only one hardware *UART*, let's check what the reference manual tells us about
it.

1. The *UBRR0H* and *UBRR0L* registers are used to set the transmission speed.
As 16 bits values are accepted but Atmega328P is an 8 bits MCU, 2 registers are
used.
1. *libavr* has macros defined in *<util/setbaud.h>* to compute at compilation
time the proper values to put in the *UBRR0H* and *UBRR0L* registers.
1. Those macros in *<util/setbaud.h>* might or might not judge that 2X mode UART 
is required. 2X mode is set with bit *U2X0* of register *UCSR0A*.
1. Three registers *UCSR0A*, *UCSR0B* and *UCSR0C* are the *USART Control and Status Register*,
are used to manage transmissions.
1. *UCSR0A*, amongst other things, gives informations on the current status of the transmissions
1. *UCSR0B*, amongst other things, enable transmission and receptions.
1. *UCSR0C*, amongst other things, controls whether stops bits and parity bits are required,
the size of the transmitted characters.
1. The *UDR0* register is the data register, used to receive or send characters.

Armed with this knowldge, we 

1. initialize the *UART* by filling registers *UBRR0H*, *UBRR0L*, *UCSR0C* and *UCSR0B*.
1. send/receive character with register *UDR0*
1. check if we can send/receive more characters with register *UCSR0A*

*libavr* provides a busy loop implementation that wait for a given bit to be set,
`loop_until_bit_is_set`

### Using stdio functions with the UART ###

*libavr* provides a nice way to setup our own streams with the 
`FDEV_SETUP_STREAM` macro. After providing a `putchar` and `getchar` function,
we get a stream, and we can call `fprintf` for example. It's what we do here
to send and receive on the *UART*.

`uart_putchar` appends a carriage return (`\r`) whenever a line return is 
entered, to signal to the serial terminal that receives our communication that
we finished the message we typed in.