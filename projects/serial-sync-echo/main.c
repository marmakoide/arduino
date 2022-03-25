#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define BAUD 9600 // Need to be defined before utils/setbaud.h inclusion
#include <util/setbaud.h>


// --- UART initialisation ----------------------------------------------------

void
uart_init(void) {
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	#if USE_2X
    	UCSR0A |= _BV(U2X0);
	#else
    	UCSR0A &= ~(_BV(U2X0));
	#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
	UCSR0B = _BV(RXEN0)  | _BV(TXEN0);  // Enable RX and TX
}


// --- Setup to use stdio function for serial communications ------------------

int
uart_putchar(char c, FILE *stream) {
    if (c == '\n')
        uart_putchar('\r', stream);
 
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

int
uart_getchar(FILE *stream) {
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}


FILE uart_output =
	FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
	
FILE uart_input =
	FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

FILE uart_io =
	FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);



// --- Main entry point -------------------------------------------------------

#define INPUT_BUFFER_SIZE 18

int
main(void) {
	char input_buffer[INPUT_BUFFER_SIZE];

	// UART setup
    uart_init();

    fputs("---[ Arduino echo ]---\n", &uart_output);
    while(1) {
    	// Fill the input buffer
        fgets(input_buffer, INPUT_BUFFER_SIZE - 1, &uart_input);

        // Write the content of the input buffer        
        size_t len = strlen(input_buffer);
        if (input_buffer[len - 1] == '\n')
       		input_buffer[len - 1] = '\0';
        	
        fprintf(&uart_output, "=> '%s'\n", input_buffer);        
    }
}
