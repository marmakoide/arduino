#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define BAUD 57600 // Need to be defined before utils/setbaud.h inclusion
#include <util/setbaud.h>


// --- Clock state and data ---------------------------------------------------

static unsigned int clock_state = 0;

static char* clock_message[2] = { "TIC", "TOC" };


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
uart_init(void) {
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


// --- Timer handling ---------------------------------------------------------

#define T1_MAX 0xFFFFUL
#define T1_PRESCALER 1024
#define T1_TICK_US (T1_PRESCALER/(F_CPU/1000000UL)) // 64us @ 16MHz
#define T1_MAX_US (T1_TICK_US * T1_MAX)             // ~4.2s @ 16MHz


static void
timer_stop(void) {
    TCCR1B = ~(_BV(CS10) | _BV(CS11) | _BV(CS12)); // Stop timer
    TIMSK1 = ~_BV(TOIE1);                          // Disable interrupt
    TIFR1 |= _BV(TOV1);                            // Clear interrupt flag
}

 
static void
timer_init(void) {
    TCCR1A = ~(_BV(WGM10) | _BV(WGM11));
    TCCR1B = ~(_BV(WGM13) | _BV(WGM12));
}

 
static void 
timer_start(unsigned long us) {
	// Setup the timer counter
    unsigned long ticks_long;
    unsigned short ticks;

    ticks_long = us / T1_TICK_US;
    if (ticks_long >= T1_MAX)
        ticks = T1_MAX;
    else
        ticks = ticks_long;
        
    TCNT1 = T1_MAX - ticks; // overflow in ticks * 1024 clock cycles
    
    // Setup the timer interrupt
    TIMSK1 |= _BV(TOIE1); // enable overflow interrupt
    TCCR1B = ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
    TCCR1B |= _BV(CS10) | _BV(CS12); // prescaler set 1024
}


// timer #1 interrupt service routine
ISR(TIMER1_OVF_vect) {
	// Update the clock state
	clock_state += 1;

	// Display the clock message
	fputs(clock_message[clock_state & 1], &uart_output);
	fputs("\r\n", &uart_output);

	// Start the timer
	timer_start(1000000UL);
}


// --- Main entry point -------------------------------------------------------

int
main(void) {
	// Setup
	uart_init();
	timer_init();
	
	// Main loop : enable interrupts and sleeps whenever there is nothing to do
	sei(); 
	timer_start(1000000UL);
	while(1)
		sleep_mode();
}
