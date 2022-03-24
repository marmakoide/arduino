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
plugged on the USB port. Check the Makefile and the serial-com script to do so.


## Notes

The program manages a 17 characters internal buffer. If you enter a string 
short enough to not fill that buffer, it will be echoed as expected.

It gets a bit more interesting when a string longer than the internal
buffer is entered. The program will echo the full content of the buffer
before you finished to enter your string.
