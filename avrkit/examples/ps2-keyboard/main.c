#include <stdio.h>


// --- USART setup ------------------------------------------------------------

#define ENABLE_USART0_TX_BUFFER 1
#define USART0_TX_BUFFER_SIZE 8

#include <avrkit/USART.h>


ISR(USART_UDRE_vect) {
    usart0__on_data_register_empty();
}

	
void
usart0__init() {    
    usart0__set_bauds(57600UL);
    usart0__set_mode_asynchronous();
    usart0__set_char_size_8();
    usart0__set_stop_bits_1();
    usart0__set_parity_none();
    
    usart0__enable_tx();
    usart0__enable_data_register_empty_interrupt();
}


int
usart0_putchar(char c, FILE *stream) {
	usart0__send_char(c);
	return 0;
}


FILE usart_output =
	FDEV_SETUP_STREAM(usart0_putchar, NULL, _FDEV_SETUP_WRITE);


// --- PS/2 keyboard handling -------------------------------------------------

#include <avrkit/drivers/ps2-keyboard.h>


ISR(INT1_vect) {
	ps2_keyboard__rx_interrupt_service();	
}


// --- Main entry point -------------------------------------------------------

int
main(void) {
	// Setup
	usart0__init();
	ps2_keyboard__init();
	
	// Main loop : enable interrupts and sleeps whenever there is nothing to do
	sei(); 
	fputs("ready\r\n", &usart_output);

	/*
	uint16_t msg_count = 0;
	while(1) {
		uint8_t code = ps2_keyboard__get_byte();
		fprintf(&usart_output, "0x%04x code = 0x%02x\r\n", msg_count, code);
		msg_count += 1;
	}
	*/
	
	while(1) {
		uint16_t scancode = ps2_keyboard__get_set2_scancode();
		
		if (scancode & SCANCODE_MASK_KEY_RELEASE) {
		    scancode &= ~SCANCODE_MASK_KEY_RELEASE;
		    fputs("released => ", &usart_output);
		}
		else
		    fputs("pressed => ", &usart_output);
		
		switch(scancode) {
			case SCANCODE_CAPS_LOCK:
				fputs("CAPS LOCK", &usart_output);
				break;

			case SCANCODE_NUM_LOCK:
				fputs("NUM LOCK", &usart_output);
				break;

			case SCANCODE_SCROLL_LOCK:
				fputs("SCROLL LOCK", &usart_output);
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
				fprintf(&usart_output, "F%d", 1 + scancode - SCANCODE_F1);
				break;

			case SCANCODE_PAUSE:
				fputs("PAUSE", &usart_output);
				break;
				
			case SCANCODE_PRINTSCREEN:
				fputs("PRINTSCREEN", &usart_output);
				break;
				
			case SCANCODE_SPACE:
				fputs("SPACE", &usart_output);
				break;

			case SCANCODE_BACKSPACE:
				fputs("BACKSPACE", &usart_output);
				break;

			case SCANCODE_ENTER:
				fputs("ENTER", &usart_output);
				break;

			case SCANCODE_ESCAPE:
				fputs("ESCAPE", &usart_output);
				break;

			case SCANCODE_TAB:
				fputs("TAB", &usart_output);
				break;

			case SCANCODE_SEMICOLON:
				fputs(";", &usart_output);
				break;

			case SCANCODE_QUOTE:
				fputs("'", &usart_output);
				break;

			case SCANCODE_BACKQUOTE:
				fputs("`", &usart_output);
				break;

			case SCANCODE_COMMA:
				fputs(",", &usart_output);
				break;

			case SCANCODE_PERIOD:
				fputs(".", &usart_output);
				break;

			case SCANCODE_SLASH:
				fputs("/", &usart_output);
				break;

			case SCANCODE_BACKSLASH:
				fputs("\\", &usart_output);
				break;

			case SCANCODE_PLUS:
				fputs("+", &usart_output);
				break;

			case SCANCODE_MINUS:
				fputs("-", &usart_output);
				break;

			case SCANCODE_EQUAL:
				fputs("=", &usart_output);
				break;

			case SCANCODE_LEFT_BRACKET:
				fputs("[", &usart_output);
				break;

			case SCANCODE_RIGHT_BRACKET:
				fputs("]", &usart_output);
				break;

			case SCANCODE_INSERT:
				fputs("INSERT", &usart_output);
				break;
								
			case SCANCODE_DELETE:
				fputs("DELETE", &usart_output);
				break;

			case SCANCODE_LEFT_ALT:
				fputs("LEFT ALT", &usart_output);
				break;
								
			case SCANCODE_RIGHT_ALT:
				fputs("RIGHT ALT", &usart_output);
				break;
							
			case SCANCODE_LEFT_CRTL:
				fputs("LEFT CRTL", &usart_output);
				break;
				
			case SCANCODE_RIGHT_CRTL:
				fputs("RIGHT CTRL", &usart_output);
				break;
				
			case SCANCODE_LEFT_SHIFT:
				fputs("LEFT SHIFT", &usart_output);
				break;
				
			case SCANCODE_RIGHT_SHIFT:
				fputs("RIGHT SHIFT", &usart_output);
				break;

			case SCANCODE_APPLICATION:
				fputs("APPLICATION", &usart_output);
				break;

			case SCANCODE_LEFT_GUI:
				fputs("LEFT GUI", &usart_output);
				break;

			case SCANCODE_RIGHT_GUI:
				fputs("RIGHT GUI", &usart_output);
				break;

			case SCANCODE_LEFT:
				fputs("LEFT", &usart_output);
				break;

			case SCANCODE_RIGHT:
				fputs("RIGHT", &usart_output);
				break;

			case SCANCODE_UP:
				fputs("UP", &usart_output);
				break;

			case SCANCODE_DOWN:
				fputs("DOWN", &usart_output);
				break;

			case SCANCODE_HOME:
				fputs("HOME", &usart_output);
				break;

			case SCANCODE_END:
				fputs("END", &usart_output);
				break;

			case SCANCODE_PAGE_UP:
				fputs("PAGE_UP", &usart_output);
				break;

			case SCANCODE_PAGE_DOWN:
				fputs("PAGE_DOWN", &usart_output);
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
				fprintf(&usart_output, "%u", scancode - SCANCODE_0);
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
				fprintf(&usart_output, "%c", scancode - SCANCODE_A + 'A');
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
				fprintf(&usart_output, "keypad %u", scancode - SCANCODE_KP_0);
				break;

			case SCANCODE_KP_ENTER:
				fputs("keypad ENTER", &usart_output);
				break;

			case SCANCODE_KP_PLUS:
				fputs("keypad +", &usart_output);
				break;

			case SCANCODE_KP_MINUS:
				fputs("keypad -", &usart_output);
				break;

			case SCANCODE_KP_PERIOD:
				fputs("keypad .", &usart_output);
				break;
				
			case SCANCODE_KP_STAR:
				fputs("keypad *", &usart_output);
				break;

			case SCANCODE_KP_SLASH:
				fputs("keypad /", &usart_output);
				break;
				
			default:
				fputs("<unknown>", &usart_output);
		}
		
		fputs("\r\n", &usart_output);
	}
}
