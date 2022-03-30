#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>

#define BAUD 9600 // Need to be defined before utils/setbaud.h inclusion
#include <util/setbaud.h>


// --- Interrupt-driven UART management ---------------------------------------

// Transmission ring buffer
#define UART_TX_BUFFER_SIZE 16
static uint8_t uart_tx_start;
static uint8_t uart_tx_end;
static char uart_tx_buffer[UART_TX_BUFFER_SIZE];

// Reception ring buffer
#define UART_RX_BUFFER_SIZE 16
static uint8_t uart_rx_start;
static uint8_t uart_rx_end;
static char uart_rx_buffer[UART_RX_BUFFER_SIZE];


// Transmission interrupt handler
ISR(USART_UDRE_vect) {
	if (uart_tx_start != uart_tx_end) {
		UDR0 = uart_tx_buffer[uart_tx_start];
		uart_tx_start = (uart_tx_start + 1) % UART_TX_BUFFER_SIZE;
	}
}


// Reception interrupt handler
ISR(USART_RX_vect) {
	uint8_t uart_rx_next_end = (uart_rx_end + 1) % UART_TX_BUFFER_SIZE;
	if (uart_rx_next_end != uart_rx_start) {
		uart_rx_buffer[uart_rx_end] = UDR0;
		uart_rx_end = uart_rx_next_end;
	}
}


void
uart_init(void) {
	// Initialize transmission buffer
	uart_tx_start = 0;
	uart_tx_end = 0;

	// Initialize reception buffer	
	uart_rx_start = 0;
	uart_rx_end = 0;

	// Setup transmission rate
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	#if USE_2X
    	UCSR0A |= _BV(U2X0);
	#else
    	UCSR0A &= ~(_BV(U2X0));
	#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // Setup data format, async transmission
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   // Enable reception and transmission

	UCSR0B |= _BV(RXCIE0); // Enable reception interrupt
	UCSR0B |= _BV(UDRIE0); // Enable transmission ready interrupt
}


// --- Setup to use stdio function for serial communications ------------------

int
uart_putchar(char c, FILE *stream) {
	// Sleeps until there is room available in the transmission buffer
	uint8_t uart_tx_next_end = (uart_tx_end + 1) % UART_TX_BUFFER_SIZE;
	while(uart_tx_next_end == uart_tx_start)
			sleep_mode();

	// Add the character in the transmission buffer
	cli();
	uart_tx_buffer[uart_tx_end] = c;
	uart_tx_end = uart_tx_next_end;
	sei();

	// Job done	
    return 0;
}

int
uart_getchar(FILE *stream) {
	char ret;
	
	// Sleeps until there are data available in the reception buffer
	while(uart_rx_start == uart_rx_end)
		sleep_mode();

	// Pick the first character in the reception buffer
	cli();
	ret = uart_rx_buffer[uart_rx_start];
	uart_rx_start = (uart_rx_start + 1) % UART_TX_BUFFER_SIZE;
	sei();

	// Job done
	return ret;
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
	sei();
	
	// Main loop
	fputs("---[ Arduino echo ]---\r\n", &uart_output);
    while(1) {
    	// Fill the input buffer
        fgets(input_buffer, INPUT_BUFFER_SIZE - 1, &uart_input);
		
        // Write the content of the input buffer        
        size_t len = strlen(input_buffer);
        if (input_buffer[len - 1] == '\n')
       		input_buffer[len - 1] = '\0';

        fprintf(&uart_output, "=> '%s'\r\n", input_buffer);        
    }
}
