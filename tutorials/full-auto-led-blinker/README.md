# full-auto-led-blinker

This example blinks a led plugged to port 9 (don't forget to put a current 
limiting resistor in serie). The blink period is a very accurate 1 second, and
it's handled while the MCU remain in sleep mode. This possible using timer 1
functionalities.


## Notes

Timer 1 is a 16 bits timer, with a precaler, ie. that will count every ticks, or
every 8 ticks, every 64 ticks, every 256 ticks or every 1024 ticks. The Arduino 
Uno runs at 16 Mhz, and we can note that 16 000 000 = 1024 * 15265. 

Timer 1 setup goes as follow

1. Ask to toggle pin OC1A (aka pin B1) when timer matches OCR1A
1. Ask to clear the tick count when timer matches OCR1A
1. Increment the timer every 1024 ticks, *TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10)*
1. Set *OCR1A = 15624*
1. Enable write operation on pin B1, as *OC1A* is associated to that pin

And this will work automatically while the MCU remains in sleep mode,
making for a very ressource efficient blinking led.

