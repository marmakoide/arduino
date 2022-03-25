# clock

This program will make the Arduino board output every second, in alternance, 
__TIC__ and __TOC__ on the serial port.

The program use a timer interruption instead of a busy loop.

  * Compile with the following command : `make`
  * Upload to the Arduino with the following command : `make upload`
  * Clean-up with the following command : `make clean`
  * Launch the serial monitor with the following command : `./serial-com`
  * You can leave the serial monitor session by pressing crtl-A, then crtl-X.
  
You might have to modify the USB device associated to the Arduino UNO when 
plugged on the USB port. Check the Makefile and the [serial-com](serial-com) 
script to do so.
