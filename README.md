# Alex's Arduino & AVR stuffs

This repository contain all my tinkerings with Arduino boards I did for my 
personal entertainment and enlightenment. The Arduino language and associated 
IDE are not used, as they do not suit my taste. Here, it's Linux, plain C, 
Makefile, and a shell.

Unless specified otherwise, those tutorials/projects are targeting the Arduino
UNO board. 


## Tutorials

The tutorials are more or less in topological order : later tutorials are 
building upon earlier tutorials. Each tutorial has some notes I took, as an 
attempt to leave the least amount of mystery.

1. [led-blinker](tutorials/led-blinker) : blinks the Arduino UNO's on-board led
1. [pin-trigger](tutorials/pin-trigger) : switch on and off the Arduino UNO's on-board led with pin 12
1. [serial-sync-echo](tutorials/serial-sync-echo) : echo on the serial output what is given in the serial input, synchronous style
1. [clock](tutorials/clock) : setup a timer to send a message on the serial output every second.
1. [ADC](tutorials/analog-read) : sample an analog input to switch on and off switch on and off the Arduino UNO's on-board led.
1. [i2c](tutorials/i2c): interfacing with i2c devices 
1. [SSD1306](https://github.com/Matiasus/SSD1306) : code for controlling SSD1306 OLED screens, easy to follow

## Software environment

### Archlinux 

On a vanilla Archlinux, install the following packages:

* [arduino-avr-core](https://archlinux.org/packages/community/any/arduino-avr-core/) C language toolchain for AVR MCUs and related utilities
* [picocom](https://archlinux.org/packages/community/x86_64/picocom/) Minimalistic serial terminal


### Ubuntu

On Ubuntu, install the following packages:

* gcc-avr
* avr-libc
* picocom

## Information sources

I gathered the information I needed to realize the tutorials from those sources

* [Atmel ATmega328P Reference Manual](https://github.com/eerimoq/hardware-reference/blob/master/Atmel/atmega328p%20reference%20manual.pdf)
* [Arduino in C](https://balau82.wordpress.com/arduino-in-c) A small series of tutorials on using plain C and AVR utilities
* [Easy UART on with avrlib](https://appelsiini.net/2011/simple-usart-with-avr-libc/) Synchronous UART handling libavr's stdio implementation
* [Bare Metal Micro](https://baremetalmicro.com/) Nicely explained examples on the fundamentals ie. ports, interrupts
* [The USART of the AVR](https://maxembedded.com/2013/09/the-usart-of-the-avr/) Detailed explanation about the USART/UART, good complement to the reference manual
* [I2C on the AVR](http://www.avrbeginners.net/architecture/twi/twi.html)

I am very thankful to all those people who took the time and the effort to
write those tutorials and manuals.


## Authors

* **Alexandre Devert** - [marmakoide](https://github.com/marmakoide)
