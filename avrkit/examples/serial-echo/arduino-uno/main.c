#include <string.h>


// --- USART setup ------------------------------------------------------------

#define ENABLE_USART0_TX_BUFFER 1
#define USART0_TX_BUFFER_SIZE 8

#define ENABLE_USART0_RX_BUFFER 1
#define USART0_RX_BUFFER_SIZE 8

#include <avrkit/USART.h>


ISR(USART_UDRE_vect) {
	usart0__on_data_register_empty();
}
	
ISR(USART_RX_vect) {
	usart0__on_rx_complete();
}


void
usart0__init() {
	usart0__set_bauds(9600UL);
	usart0__set_mode_asynchronous();
	usart0__set_char_size_8();
	usart0__set_stop_bits_1();
	usart0__set_parity_none();
    
	usart0__enable_rx();
	usart0__enable_rx_complete_interrupt();
    
	usart0__enable_tx();
	usart0__enable_data_register_empty_interrupt();
}


// --- Main entry point -------------------------------------------------------

#define INPUT_BUFFER_SIZE 17
	
int
main(void) {
	char input_buffer[INPUT_BUFFER_SIZE];
    
	usart0__init();
	sei();
	
	
	// Main loop
	usart0__send_str("---[ Arduino echo ]---\r\n");
	while(1) {
		// Fill the input buffer
		usart0__get_line(input_buffer, INPUT_BUFFER_SIZE);
		
		// Write the content of the input buffer
		usart0__send_str("=> '");
		usart0__send_str(input_buffer);
		usart0__send_str("'\r\n");
	}
}


