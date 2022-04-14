#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define BAUD 9600 // Need to be defined before utils/setbaud.h inclusion
#include <util/setbaud.h>


// --- UART handling ----------------------------------------------------------

// Transmission ring buffer
#define UART_TX_BUFFER_SIZE 16
static volatile uint8_t uart_tx_start;
static volatile uint8_t uart_tx_end;
static volatile char uart_tx_buffer[UART_TX_BUFFER_SIZE];


// Transmission interrupt handler
ISR(USART_UDRE_vect) {
	if (uart_tx_start != uart_tx_end) {
		UDR0 = uart_tx_buffer[uart_tx_start];
		uart_tx_start = (uart_tx_start + 1) % UART_TX_BUFFER_SIZE;
	}
}


void
uart_init() {
	// Initialize transmission buffer
	uart_tx_start = 0;
	uart_tx_end = 0;

	// Setup transmission rate
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	#if USE_2X
    	UCSR0A |= _BV(U2X0);
	#else
    	UCSR0A &= ~(_BV(U2X0));
	#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // Setup data format, async transmission
	UCSR0B = _BV(TXEN0);   // Enable reception and transmission

	UCSR0B |= _BV(UDRIE0); // Enable transmission ready interrupt
}


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


FILE uart_output =
	FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

	
// --- TWI handling -----------------------------------------------------------

#define F_SCL 400000UL // Clock frequency for I2C protocol

void
twi_init() {
    // Set pin 0 for write operation, set high
    DDRC |= _BV(DDC4);
    PORTC |= _BV(PORTC4);

    // Set pin 1 for write operation, set high    
    DDRC |= _BV(DDC5);
    PORTC |= _BV(PORTC5);
    
    // TWI registers setup
    TWBR = (uint8_t)(((F_CPU / F_SCL) - 16) / 2);
}


void
twi_start() {
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
}


void
twi_stop() {
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
}


void
twi_send_slave_address(uint8_t address) {
    TWDR = (address << 1) | TW_WRITE;
	TWCR = (1 << TWINT) | (1 << TWEN);
    loop_until_bit_is_set(TWCR, TWINT);	
}


// --- Main entry point -------------------------------------------------------

int
main() {
    uint8_t id_found_count;
    uint8_t id_found_set[16];
    
	// Setup
	uart_init();
	twi_init();
	
	// I2C bus scanning
	id_found_count = 0;
	memset(id_found_set, 0, 16);
	
	fputs("Scanning I2C bus ...\r\n", &uart_output);
	for(uint8_t i = 1; i < 128; ++i) {
	    // Send START message on the I2C bus
	    twi_start();
	    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START)) {
	        fprintf(&uart_output, "  => I2C 'start' message failed (status = 0x%02x)\r\n", TW_STATUS);
	        goto waiting_loop;
	    }
	    
	    // Send slave address on the I2C bus
	    twi_send_slave_address(i);
	    if (TW_STATUS == TW_MT_SLA_ACK) {
	        id_found_set[i / 8] |= 1 << (i % 8);
	        id_found_count += 1;
	    }	   
	}
	fputs("\r\nScanning complete\r\n", &uart_output);
	
	// List the devices found
	if (id_found_count == 0)
	    fputs("No devices found\r\n", &uart_output);
	else {
	    fprintf(&uart_output, "%u device(s) found:\r\n", id_found_count);
	    for(uint8_t i = 1; i < 128; ++i) {
	        if (id_found_set[i / 8] & (1 << (i % 8)))
	            fprintf(&uart_output, "  0x%02x\r\n", i);
        }
	}
	
	// Wait, do nothing loop
    waiting_loop:
	while(1) {
	    sleep_mode();
	}
}
