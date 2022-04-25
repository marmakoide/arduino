# avrkit

This is a C header-only library to supplement GCC's libc and avrlib. 

Its main objectives are 
* rewriting my Arduino tutorials in a way that makes the code a bit more 
self-explanatory
* being able to reuse code to drive specific devices connected to the MCY
* little to no compromises compared to direct access of registers, using macros 
and function inlining

It has non-features such as 

* currently only working and tested on ATmega328p
* only covers a small fraction of the functionalities of the AVR MCUs
* maintened in the late evening by a tired dad having a beer
* horrific compilation error messages thanks to macro-heavy implementation

C++ templates would be a technically superior choice, but I want to be able to 
repurpose this code for other microcontrollers with no C++ toolchains 
available, such as the STM8. Code generation would be the best ie. using a
separate, fancier macro-processor than the stock one bundled with a standard
C compiler.
