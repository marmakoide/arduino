# Arduino tutorial projects

This repository is a set of small educational Arduino projects I did for my 
personal entertainment and enlightenment. The Arduino language and associated 
IDE are not used, as they do not suit my taste. Here, it's Linux, plain C, 
Makefile, and a shell.

Unless specified otherwise, those projects are targeting the Arduino UNO board. 


## Software environment

I did those project on a vanilla Archlinux install. I installed the following 
packages:

* [arduino-avr-core](https://archlinux.org/packages/community/any/arduino-avr-core/) C language toolchain for AVR MCUs and related utilities
* [picocom](https://archlinux.org/packages/community/x86_64/picocom/) Minimalistic serial terminal


## Information sources

I gathered information there

* [Atmel ATmega328P Reference Manual](https://github.com/eerimoq/hardware-reference/blob/master/Atmel/atmega328p%20reference%20manual.pdf)
* [Arduino in C](https://balau82.wordpress.com/arduino-in-c) A small series of tutorials on using plain C and AVR utilities
* [Easy UART on with avrlib](https://appelsiini.net/2011/simple-usart-with-avr-libc/) Synchronous UART handling libavr's stdio implementation

## Projects

* [led-blinker](projects/led-blinker) : blinks the Arduino UNO on-board led
* [serial-sync-echo](projects/serial-sync-echo) : echo on the serial output what is given in the serial input, synchronous style
* [clock](projects/clock) : setup a timer to send a message on the serial output every second.

## Authors

* **Alexandre Devert** - [marmakoide](https://github.com/marmakoide)
