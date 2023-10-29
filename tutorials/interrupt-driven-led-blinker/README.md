# interrupt-driven-led-blinker

This example blinks the on-board led of the Arduino Uno. The blink period is 
a very accurate 1 second, and it is interrupt driven.

## Notes

This tutorial build upons the [full-auto-led-blinker](tutorials/full-auto-led-blinker)
tutorial. Instead of toggling the pin *OC1A* aka pin *B1* automatically when
timer 1 matches the value stored in *OCR1A*, we trigger the *TIMER_COMPA*
interrupt. In that interruption, we simply toggle the state of the pin *B5*.
