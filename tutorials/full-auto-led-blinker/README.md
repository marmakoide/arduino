# full-auto-led-blinker

This example blinks a led plugged to port 9 (don't forget to put a current 
limiting resistor in serie). The blink period is a very accurate 1 second, and
it's handled while the MCU remain in sleep mode. 

## Notes

A timer is a counter incremented every N clock ticks. N can be either 1,
8, 64 256 or 1024 : it's the *prescaler*, defined in register *TCCR1B* for
timer 1, bits *CS10*, *CS11* and *CS12*. There's even an option to count
ticks from an external input.

Timer 1 can run in CTC mode : Clear Timer on Compare Match. In this mode, when
the timer count becomes equals the value stored in the 16 bites register *OCR1A*,
the timer is reset to zero. Some further actions can be associated to this event
like toggling pin *OC1A* aka pin *B1*, port 9 on an Arduino Uno.

We observe that *16 000 000 = 15625 x 1024*, thus setting *OCR1A* will toggle
the *OC1A* pin every second with the prescaler set to 1024.

Timer 1 setup goes as follow

1. Ask to toggle pin OC1A (aka pin B1) when timer matches OCR1A
1. Ask to clear the tick count when timer matches OCR1A, aka *CTC mode*
1. Increment the timer every 1024 ticks, *TCCR1B |= (1 << CS12)*
1. Set *OCR1A = 15624*
1. Enable write operation on pin B1, as *OC1A* is associated to that pin
