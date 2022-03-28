# pin-trigger

This is a minimal example to introduct interruptions : when the pin 12 of the 
Arduino UNO is connected to GND, the on-board led is on. When the connection 
is removed, the on-board led is off. This is done without polling ie. without 
a  busy loop constantly checking for pin 12 state. Instead, the MPU is sleeping
almost all the time, and wake up only when pin 12 state change, thanks to
an interrupt mechanism.

## Notes

### Handling events on a pin

After the [led-blinker](../projects/led-blinker) project, we are familiar with
setting the on-board led on and off.

Pin 12 is controled by port *B*, bit 4. Since we are going to read the state of
that pin, we set it for read operations. We also enable the pull-up resistor of
the pin,.

From the reference manual, we can see that port *B*, bit 4 is associated to
*PCINT4*. We can now setup a detection of any changes on port *B*, bit 4,
with the following step:

1. *PCICR*, aka *Pin Change Interrupt Control Register*, controls which group 
of pins will be monitored for state changes. Setting the bit 0 of *PCICR* 
(which is called *PCIE0* aka *Pin Change Interrupt Enable 0*) will enable the 
monitoring of pins controled by *PCINT0* to *PCINT7*.
1. *PCMSK0*, aka *Pin Change Mask 0*, controls which pins will be monitored
for state changes within a group selected with *PCICR*. Here, we select
*PCINT4*.

After this setup sequence, once interrupts are allowed, each time pin 4 changes,
interruption vector *PCINT0* is called. In *PCINT0*, we read the state of 
port *B*, bit 4 and switch the on-board accordingly.

### Sleep mode

The call `sleep_mode()` defined in *<avr/sleep.h>* will put the MPU in a state
of low power usage, with some parts of it deactived. The MPU will wake up to
process interrupts. Typically, we want to spend as much time as possible in
sleep mode to save power.