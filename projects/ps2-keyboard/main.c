#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define BAUD 57600 // Need to be defined before utils/setbaud.h inclusion
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
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   // Enable reception and transmission

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


// --- PS/2 keyboard handling -------------------------------------------------

// Reception ring buffer
#define PS2_KEYBOARD_RX_BUFFER_SIZE 16

static volatile uint8_t ps2_keyboard_rx_start;
static volatile uint8_t ps2_keyboard_rx_end;
static volatile uint8_t ps2_keyboard_rx_buffer[PS2_KEYBOARD_RX_BUFFER_SIZE];

// ps2 keyboard interrupt handler
ISR(INT1_vect) {
    // PS/2 keyboard interrupt handler state
    static uint8_t bit_count = 0;
    static uint8_t bit_buffer = 0;
    static uint8_t odd_parity_bit = 0;

    // Read state on port D, pin 4
    volatile uint8_t incoming_bit = bit_is_set(PIND, PIND4) != 0;
    
    // Process the incoming bit
    switch(bit_count) {
        case 0x00: // start bit => should always be 0
            bit_count += 1 - incoming_bit;
            break;
            
        case 0x01: // data bit => eight of them
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
            bit_buffer |= incoming_bit << (bit_count - 1);
            odd_parity_bit = (odd_parity_bit + incoming_bit) & 0x01;
            bit_count += 1;
            break;
            
        case 0x09: // odd parity bit
            incoming_bit = 1 - incoming_bit;
            if (odd_parity_bit != incoming_bit) {
                bit_count = 0;
                odd_parity_bit = 0;
                bit_buffer = 0;
            }
            else
                bit_count += 1;            
            break;
            
        case 0x0a: // stop bit => should always be 1
            uint8_t ps2_keyboard_rx_next_end = (ps2_keyboard_rx_end + 1) % UART_TX_BUFFER_SIZE;
            if (ps2_keyboard_rx_next_end != ps2_keyboard_rx_start) {
            	ps2_keyboard_rx_buffer[ps2_keyboard_rx_end] = bit_buffer;
            	ps2_keyboard_rx_end = ps2_keyboard_rx_next_end;
            }
            	
            bit_count = 0;
            odd_parity_bit = 0;
            bit_buffer = 0;
            break;
            
        default: // should never happen
            break;
    } // switch(bit_count)
}


// ps2 keyboard handling initialization
void
ps2_keyboard_init() {
	// Initialize reception buffer	
	ps2_keyboard_rx_start = 0;
	ps2_keyboard_rx_end = 0;
	
	// Setup for Arduino pin 3 aka ATmega368 port D, pin 3 => PS/2 clock signal
	DDRD &= ~_BV(DDD3);    // Set pin 3 of port D (Arduino pin 3) for input
	PORTD |= _BV(PORTD3);  // Enable pull-up for pin 3 of port D
	EICRA |= _BV(ISC11);   // Interrupt on falling signal
	EICRA &= ~_BV(ISC10);   	
	EIMSK |= _BV(INT1);    // Port D, pin 3 is serviced by external interrupt 1

	// Setup for Arduino pin 4 aka ATmega368 port D, pin 4 => PS/2 data signal
	DDRD &= ~_BV(DDD4);   // Set pin 4 of PORTD (Arduino pin 4) for input
	PORTD |= _BV(PORTD4); // Enable pull-up for pin 4 of port D
}


// read raw data from the ps2 keyboard
uint8_t
ps2_keyboard_read_byte() {
	// Sleeps until there are data available in the reception buffer
	while(ps2_keyboard_rx_start == ps2_keyboard_rx_end)
		sleep_mode();
		
	// Pick the first character in the reception buffer
	cli();
	uint8_t ret = ps2_keyboard_rx_buffer[ps2_keyboard_rx_start];
	ps2_keyboard_rx_start = (ps2_keyboard_rx_start + 1) % PS2_KEYBOARD_RX_BUFFER_SIZE;
	sei();

	// Job done
	return ret;
}

// read scancodes from the ps2 keyboard
enum ps2_keyboard_scancode {
	SCANCODE_CAPS_LOCK = 1,
	SCANCODE_NUM_LOCK,
	SCANCODE_SCROLL_LOCK,
	
	SCANCODE_F1,
	SCANCODE_F2,
	SCANCODE_F3,
	SCANCODE_F4,
	SCANCODE_F5,
	SCANCODE_F6,
	SCANCODE_F7,
	SCANCODE_F8,
	SCANCODE_F9,
	SCANCODE_F10,
	SCANCODE_F11,
	SCANCODE_F12,

	SCANCODE_SPACE,
	SCANCODE_BACKSPACE,	
	SCANCODE_ENTER,
	SCANCODE_ESCAPE,
	SCANCODE_TAB,
	SCANCODE_SEMICOLON,
	SCANCODE_QUOTE,
	SCANCODE_BACKQUOTE,
	SCANCODE_COMMA,
	SCANCODE_PERIOD,
	SCANCODE_SLASH,
	SCANCODE_BACKSLASH,	
	SCANCODE_PLUS,
	SCANCODE_MINUS,
	SCANCODE_EQUAL,
	SCANCODE_RIGHT_BRACKET,
	SCANCODE_LEFT_BRACKET,
	
	SCANCODE_LEFT_ALT,
	SCANCODE_RIGHT_ALT,
	SCANCODE_LEFT_CRTL,
	SCANCODE_RIGHT_CRTL,
	SCANCODE_LEFT_SHIFT,
	SCANCODE_RIGHT_SHIFT,
	
	SCANCODE_0,
	SCANCODE_1,
	SCANCODE_2,
	SCANCODE_3,
	SCANCODE_4,
	SCANCODE_5,
	SCANCODE_6,
	SCANCODE_7,
	SCANCODE_8,
	SCANCODE_9,
	
	SCANCODE_A,
	SCANCODE_B,
	SCANCODE_C,
	SCANCODE_D,
	SCANCODE_E,
	SCANCODE_F,
	SCANCODE_G,
	SCANCODE_H,
	SCANCODE_I,
	SCANCODE_J,
	SCANCODE_K,
	SCANCODE_L,
	SCANCODE_M,
	SCANCODE_N,
	SCANCODE_O,
	SCANCODE_P,	
	SCANCODE_Q,	
	SCANCODE_R,
	SCANCODE_S,
	SCANCODE_T,
	SCANCODE_U,
	SCANCODE_V,
	SCANCODE_W,
	SCANCODE_X,
	SCANCODE_Y,
	SCANCODE_Z,

	SCANCODE_KP_0,
	SCANCODE_KP_1,
	SCANCODE_KP_2,
	SCANCODE_KP_3,
	SCANCODE_KP_4,
	SCANCODE_KP_5,
	SCANCODE_KP_6,
	SCANCODE_KP_7,
	SCANCODE_KP_8,
	SCANCODE_KP_9,
	SCANCODE_KP_PLUS,
	SCANCODE_KP_MINUS,
	SCANCODE_KP_PERIOD,
	SCANCODE_KP_STAR
};


const __flash uint8_t 
scancode_array[0x84] = {
	0,                      // 0x00
	SCANCODE_F9,            // 0x01
	0,                      // 0x02
	SCANCODE_F5,            // 0x03
	SCANCODE_F3,            // 0x04
	SCANCODE_F1,            // 0x05
	SCANCODE_F2,            // 0x06
	SCANCODE_F12,           // 0x07
	0,                      // 0x08
	SCANCODE_F10,           // 0x09
	SCANCODE_F8,            // 0x0a
	SCANCODE_F6,            // 0x0b
	SCANCODE_F4,            // 0x0c
	SCANCODE_TAB,           // 0x0d
	SCANCODE_BACKQUOTE,     // 0x0e
	0,                      // 0x0f
	0,                      // 0x10
	SCANCODE_LEFT_ALT,      // 0x11
	SCANCODE_LEFT_SHIFT,    // 0x12
	0,                      // 0x13
	SCANCODE_LEFT_CRTL,     // 0x14
	SCANCODE_Q,             // 0x15
	SCANCODE_1,             // 0x16
	0,                      // 0x17
	0,                      // 0x18
	0,                      // 0x19
	SCANCODE_Z,             // 0x1a
	SCANCODE_S,             // 0x1b
	SCANCODE_A,             // 0x1c
	SCANCODE_W,             // 0x1d
	SCANCODE_2,             // 0x1e
	0,                      // 0x1f
	0,                      // 0x20
	SCANCODE_C,             // 0x21
	SCANCODE_X,             // 0x22
	SCANCODE_D,             // 0x23
	SCANCODE_E,             // 0x24
	SCANCODE_4,             // 0x25
	SCANCODE_3,             // 0x26
	0,                      // 0x27
	0,                      // 0x28
	SCANCODE_SPACE,         // 0x29
	SCANCODE_V,             // 0x2a
	SCANCODE_F,             // 0x2b
	SCANCODE_T,             // 0x2c
	SCANCODE_R,             // 0x2d
	SCANCODE_5,             // 0x2e
	0,                      // 0x2f
	0,                      // 0x30
	SCANCODE_N,             // 0x31
	SCANCODE_B,             // 0x32
	SCANCODE_H,             // 0x33
	SCANCODE_G,             // 0x34
	SCANCODE_Y,             // 0x35
	SCANCODE_6,             // 0x36
	SCANCODE_7,             // 0x37
	0,                      // 0x38
	0,                      // 0x39
	SCANCODE_M,             // 0x3a
	SCANCODE_J,             // 0x3b
	SCANCODE_U,             // 0x3c
	SCANCODE_7,             // 0x3d
	SCANCODE_8,             // 0x3e
	0,                      // 0x3f
	0,                      // 0x40
	SCANCODE_COMMA,         // 0x41
	SCANCODE_K,             // 0x42
	SCANCODE_I,             // 0x43
	SCANCODE_O,             // 0x44
	SCANCODE_0,             // 0x45
	SCANCODE_9,             // 0x46
	0,                      // 0x47
	0,                      // 0x48
	SCANCODE_PERIOD,        // 0x49
	SCANCODE_SLASH,         // 0x4a
	SCANCODE_L,             // 0x4b
	SCANCODE_SEMICOLON,     // 0x4c
	SCANCODE_P,             // 0x4d
	SCANCODE_MINUS,         // 0x4e
	0,                      // 0x4f
	0,                      // 0x50
	0,                      // 0x51
	SCANCODE_QUOTE,         // 0x52
	0,                      // 0x53
	SCANCODE_LEFT_BRACKET,  // 0x54
	SCANCODE_EQUAL,         // 0x55
	0,                      // 0x56
	0,                      // 0x57
	SCANCODE_CAPS_LOCK,     // 0x58
	SCANCODE_RIGHT_SHIFT,   // 0x59
	SCANCODE_ENTER,         // 0x5a
	SCANCODE_RIGHT_BRACKET, // 0x5b
	0,                      // 0x5c
	SCANCODE_BACKSLASH,     // 0x5d
	0,                      // 0x5e
	0,                      // 0x5f
	0,                      // 0x60
	0,                      // 0x61
	0,                      // 0x62
	0,                      // 0x63
	0,                      // 0x64
	0,                      // 0x65
	SCANCODE_BACKSPACE,     // 0x66
	0,                      // 0x67
	0,                      // 0x68
	SCANCODE_KP_1,          // 0x69
	0,                      // 0x6a
	SCANCODE_KP_4,          // 0x6b
	SCANCODE_KP_7,          // 0x6c
	0,                      // 0x6d
	0,                      // 0x6e
	0,                      // 0x6f
	SCANCODE_KP_0,          // 0x70
	SCANCODE_KP_PERIOD,     // 0x71
	SCANCODE_KP_2,          // 0x72
	SCANCODE_KP_5,          // 0x73
	SCANCODE_KP_6,          // 0x74
	SCANCODE_KP_8,          // 0x75
	SCANCODE_ESCAPE,        // 0x76
	SCANCODE_NUM_LOCK,      // 0x77
	SCANCODE_F11,           // 0x78
	SCANCODE_KP_PLUS,       // 0x79
	SCANCODE_KP_3,          // 0x7a
	SCANCODE_KP_MINUS,      // 0x7b
	SCANCODE_KP_STAR,       // 0x7c
	SCANCODE_KP_9,          // 0x7d
	SCANCODE_SCROLL_LOCK,   // 0x7e
	0,                      // 0x7f
	0,                      // 0x80
	0,                      // 0x81
	0,                      // 0x82
	SCANCODE_F7             // 0x83
};


uint16_t
ps2_keyboard_read_scancode() {
	uint16_t ret = 0x0000;

	uint8_t code = ps2_keyboard_read_byte();
	if ((code == 0xf0) || (code == 0xe0))
		code = ps2_keyboard_read_byte();
	
	if (code < sizeof(scancode_array))
		ret = scancode_array[code];
	
	return ret;
}


// --- Main entry point -------------------------------------------------------

int
main(void) {
	// Setup
	uart_init();
	ps2_keyboard_init();
	
	// Main loop : enable interrupts and sleeps whenever there is nothing to do
	sei(); 

	//uint16_t msg_count = 0;
	while(1) {
		uint16_t scancode = ps2_keyboard_read_scancode();
		switch(scancode) {
			case SCANCODE_CAPS_LOCK:
				fputs("CAPS LOCK\r\n", &uart_output);
				break;

			case SCANCODE_NUM_LOCK:
				fputs("NUM LOCK\r\n", &uart_output);
				break;

			case SCANCODE_SCROLL_LOCK:
				fputs("SCROLL LOCK\r\n", &uart_output);
				break;
								
			case SCANCODE_F1:
			case SCANCODE_F2:	
			case SCANCODE_F3:
			case SCANCODE_F4:
			case SCANCODE_F5:
			case SCANCODE_F6:
			case SCANCODE_F7:
			case SCANCODE_F8:
			case SCANCODE_F9:
			case SCANCODE_F10:
			case SCANCODE_F11:
			case SCANCODE_F12:			
				fprintf(&uart_output, "F%d\r\n", 1 + scancode - SCANCODE_F1);
				break;

			case SCANCODE_SPACE:
				fputs("SPACE\r\n", &uart_output);
				break;

			case SCANCODE_BACKSPACE:
				fputs("BACKSPACE\r\n", &uart_output);
				break;

			case SCANCODE_ENTER:
				fputs("ENTER\r\n", &uart_output);
				break;

			case SCANCODE_ESCAPE:
				fputs("ESCAPE\r\n", &uart_output);
				break;

			case SCANCODE_TAB:
				fputs("TAB\r\n", &uart_output);
				break;

			case SCANCODE_SEMICOLON:
				fputs(";\r\n", &uart_output);
				break;

			case SCANCODE_QUOTE:
				fputs("'\r\n", &uart_output);
				break;

			case SCANCODE_BACKQUOTE:
				fputs("`\r\n", &uart_output);
				break;

			case SCANCODE_COMMA:
				fputs(",\r\n", &uart_output);
				break;

			case SCANCODE_PERIOD:
				fputs(".\r\n", &uart_output);
				break;

			case SCANCODE_SLASH:
				fputs("/\r\n", &uart_output);
				break;

			case SCANCODE_BACKSLASH:
				fputs("\\\r\n", &uart_output);
				break;

			case SCANCODE_PLUS:
				fputs("+\r\n", &uart_output);
				break;

			case SCANCODE_MINUS:
				fputs("-\r\n", &uart_output);
				break;

			case SCANCODE_EQUAL:
				fputs("=\r\n", &uart_output);
				break;

			case SCANCODE_LEFT_BRACKET:
				fputs("[\r\n", &uart_output);
				break;

			case SCANCODE_RIGHT_BRACKET:
				fputs("]\r\n", &uart_output);
				break;

			case SCANCODE_LEFT_ALT:
				fputs("LEFT ALT\r\n", &uart_output);
				break;
								
			case SCANCODE_RIGHT_ALT:
				fputs("RIGHT ALT\r\n", &uart_output);
				break;
							
			case SCANCODE_LEFT_CRTL:
				fputs("LEFT CRTL\r\n", &uart_output);
				break;
				
			case SCANCODE_RIGHT_CRTL:
				fputs("RIGHT CTRL\r\n", &uart_output);
				break;
				
			case SCANCODE_LEFT_SHIFT:
				fputs("LEFT SHIFT\r\n", &uart_output);
				break;
				
			case SCANCODE_RIGHT_SHIFT:
				fputs("RIGHT SHIFT\r\n", &uart_output);
				break;
					
			case SCANCODE_0:
			case SCANCODE_1:
			case SCANCODE_2:
			case SCANCODE_3:
			case SCANCODE_4:
			case SCANCODE_5:
			case SCANCODE_6:
			case SCANCODE_7:
			case SCANCODE_8:
			case SCANCODE_9:
				fprintf(&uart_output, "%u\r\n", scancode - SCANCODE_0);
				break;
				
			case SCANCODE_A:
			case SCANCODE_B:
			case SCANCODE_C:
			case SCANCODE_D:
			case SCANCODE_E:
			case SCANCODE_F:
			case SCANCODE_G:
			case SCANCODE_H:
			case SCANCODE_I:
			case SCANCODE_J:
			case SCANCODE_K:
			case SCANCODE_L:
			case SCANCODE_M:
			case SCANCODE_N:
			case SCANCODE_O:
			case SCANCODE_P:
			case SCANCODE_Q:
			case SCANCODE_R:
			case SCANCODE_S:
			case SCANCODE_T:
			case SCANCODE_U:
			case SCANCODE_V:
			case SCANCODE_W:
			case SCANCODE_X:
			case SCANCODE_Y:
			case SCANCODE_Z:
				fprintf(&uart_output, "%c\r\n", scancode - SCANCODE_A + 'A');
				break;

			case SCANCODE_KP_0:
			case SCANCODE_KP_1:
			case SCANCODE_KP_2:
			case SCANCODE_KP_3:
			case SCANCODE_KP_4:
			case SCANCODE_KP_5:
			case SCANCODE_KP_6:
			case SCANCODE_KP_7:
			case SCANCODE_KP_8:
			case SCANCODE_KP_9:
				fprintf(&uart_output, "keypad %u\r\n", scancode - SCANCODE_KP_0);
				break;

			case SCANCODE_KP_PLUS:
				fputs("keypad +\r\n", &uart_output);
				break;

			case SCANCODE_KP_MINUS:
				fputs("keypad +\r\n", &uart_output);
				break;

			case SCANCODE_KP_PERIOD:
				fputs("keypad .\r\n", &uart_output);
				break;
				
			case SCANCODE_KP_STAR:
				fputs("keypad *\r\n", &uart_output);
				break;
				
			default:
				fputs("<unknown>\r\n", &uart_output);
		}
		
		//fprintf(&uart_output, "0x%04x code = 0x%02x\r\n", msg_count, code);
		//msg_count += 1;
	}
}
