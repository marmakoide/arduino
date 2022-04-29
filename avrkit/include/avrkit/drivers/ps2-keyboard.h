#ifndef AVRKIT_DRIVER_PS2_KEYBOARD_H
#define AVRKIT_DRIVER_PS2_KEYBOARD_H

#include <avr/sleep.h>
#include <avr/interrupt.h>

#include <avrkit/GPIO.h>

// If you set PS2_KEYBOARD_RX_BUFFER_SIZE, take at least 8 and preferably a power of 2
// Power of 2 is preferaqble for performance
// 8 is the length of longest scancode sequence that one key press can trigger
#define DEFAULT_PS2_KEYBOARD_RING_BUFFER_SIZE 16


// Reception ring buffer management
#define PS2_KEYBOARD_RX_BUFFER(STRUCT_NAME, BUFFER_SIZE) \
STATIC_RING_BUFFER_TYPE(STRUCT_NAME, char, BUFFER_SIZE)

// Instanciation of the reception ring buffer
#ifndef PS2_KEYBOARD_RX_BUFFER_SIZE
	#define PS2_KEYBOARD_RX_BUFFER_SIZE DEFAULT_PS2_KEYBOARD_RING_BUFFER_SIZE
#endif

#define INSTANCIATE_PS2_KEYBOARD_RX_BUFFER \
  PS2_KEYBOARD_RX_BUFFER(ps2_keyboard__RxRingBuffer, PS2_KEYBOARD_RX_BUFFER_SIZE)
  
INSTANCIATE_PS2_KEYBOARD_RX_BUFFER


// Initialisation routine
static void
ps2_keyboard__init() {
	gpio_pin_D3__set_as_input();
	gpio_pin_D3__set_high();

	EICRA |= 1 << ISC11;
	EICRA &= ~(1 << ISC10);
	EIMSK |= 1 << INT1;
	
	gpio_pin_D4__set_as_input();
	gpio_pin_D4__set_high();		
}


// Interrupt service handler
inline static void
ps2_keyboard__rx_interrupt_service() {
	// Interrupt handler state
	static uint8_t bit_count = 0;
	static uint8_t bit_buffer = 0;
	static uint8_t odd_parity_bit = 0;
	
	// Read state on port D, pin 4
	volatile uint8_t incoming_bit = gpio_pin_D4__is_high() != 0;

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
			
			// Reset the interrupt handler if the parity bit is wrong
			if (odd_parity_bit != incoming_bit) {
				bit_count = 0;
				odd_parity_bit = 0;
				bit_buffer = 0;
			}
			else
				bit_count += 1;
			break;

		case 0x0a: // stop bit => should always be 1
			// Push the byte to the ring buffer if not full
			if (ps2_keyboard__RxRingBuffer__full())
				; // TODO : should signal when the ring buffer is full
			else
				ps2_keyboard__RxRingBuffer__push(bit_buffer);
			
			// Reset the interrupt handler state
			bit_count = 0;
			odd_parity_bit = 0;
			bit_buffer = 0;
			break;
		
		default: // should never happen
			break;
	} // switch(bit_count)
}


// Reception of raw data from the ps2 keyboard
inline static uint8_t
ps2_keyboard__get_byte() {
	return ps2_keyboard__RxRingBuffer__atomic_pop_or_sleep();
}


// read scancodes from the ps2 keyboard
enum ps2_keyboard__scancode {
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
	SCANCODE_INSERT,
    SCANCODE_DELETE,
    SCANCODE_END,
    SCANCODE_HOME,
    SCANCODE_PAGE_DOWN,
    SCANCODE_PAGE_UP,
    
	SCANCODE_LEFT_ALT,
	SCANCODE_RIGHT_ALT,
	SCANCODE_LEFT_CRTL,
	SCANCODE_RIGHT_CRTL,
	SCANCODE_LEFT_SHIFT,
	SCANCODE_RIGHT_SHIFT,
	
	SCANCODE_LEFT,
	SCANCODE_RIGHT,
	SCANCODE_UP,
	SCANCODE_DOWN,

	SCANCODE_PAUSE,
	SCANCODE_PRINTSCREEN,
	
	SCANCODE_LEFT_GUI,
	SCANCODE_RIGHT_GUI,
    SCANCODE_APPLICATION,
	
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
	SCANCODE_KP_STAR,
	SCANCODE_KP_SLASH,
	SCANCODE_KP_ENTER,
	
    SCANCODE_ACPI_POWER,
    SCANCODE_ACPI_SLEEP,
    SCANCODE_ACPI_WAKE,

    SCANCODE_MEDIA_PREV_TRACK,
    SCANCODE_MEDIA_VOLUME_DOWN,
    SCANCODE_MEDIA_MUTE,
    SCANCODE_MEDIA_CALCULATOR,
    SCANCODE_MEDIA_VOLUME_UP,
    SCANCODE_MEDIA_PLAY_PAUSE,
    SCANCODE_MEDIA_PAUSE,
    SCANCODE_MEDIA_COMPUTER,
    SCANCODE_MEDIA_EMAIL,
    SCANCODE_MEDIA_NEXT_TRACK,
    SCANCODE_MEDIA_SELECT,
    SCANCODE_MEDIA_WWW_SEARCH,
    SCANCODE_MEDIA_WWW_BACK, 
    SCANCODE_MEDIA_WWW_HOME,
    SCANCODE_MEDIA_WWW_FAVORITE,
    SCANCODE_MEDIA_WWW_REFRESH, 
    SCANCODE_MEDIA_WWW_STOP,
    SCANCODE_MEDIA_WWW_FORWARD,
}; // enum ps2_keyboard_scancode


const __flash uint8_t 
ps2_keyboard__set2_scancode_array[0x84] = {
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
}; // uint8_t ps2_keyboard__set2_scancode_array[]


const __flash uint8_t 
ps2_keyboard__set2_extended_scancode_array[0x7e] = {
    0,                           // 0x00
    0,                           // 0x01
    0,                           // 0x02
    0,                           // 0x03
    0,                           // 0x04
    0,                           // 0x05
    0,                           // 0x06
    0,                           // 0x07
    0,                           // 0x08
    0,                           // 0x09
    0,                           // 0x0a
    0,                           // 0x0b
    0,                           // 0x0c
    0,                           // 0x0d
    0,                           // 0x0e
    0,                           // 0x0f

    SCANCODE_MEDIA_WWW_SEARCH,   // 0x10
    SCANCODE_RIGHT_ALT,          // 0x11
    SCANCODE_RIGHT_SHIFT,        // 0x12 (aka "fake shift" introduced by some keyboards)
    0,                           // 0x13
    SCANCODE_RIGHT_CRTL,         // 0x14
    SCANCODE_MEDIA_PREV_TRACK,   // 0x15
    0,                           // 0x16
    0,                           // 0x17
    SCANCODE_MEDIA_WWW_FAVORITE, // 0x18
    0,                           // 0x19
    0,                           // 0x1a
    0,                           // 0x1b
    0,                           // 0x1c
    0,                           // 0x1d
    0,                           // 0x1e
    SCANCODE_LEFT_GUI,           // 0x1f
    
    SCANCODE_MEDIA_WWW_REFRESH,  // 0x20
    SCANCODE_MEDIA_VOLUME_DOWN,  // 0x21
    0,                           // 0x22
    SCANCODE_MEDIA_MUTE,         // 0x23
    0,                           // 0x24
    0,                           // 0x25
    0,                           // 0x26
    SCANCODE_RIGHT_GUI,          // 0x27
    SCANCODE_MEDIA_WWW_STOP,     // 0x28
    0,                           // 0x29
    0,                           // 0x2a
    SCANCODE_MEDIA_CALCULATOR,   // 0x2b
    0,                           // 0x2c
    0,                           // 0x2d
    0,                           // 0x2e
    SCANCODE_APPLICATION,        // 0x2f
    
    SCANCODE_MEDIA_WWW_FORWARD,  // 0x30
    0,                           // 0x31
    SCANCODE_MEDIA_VOLUME_UP,    // 0x32
    0,                           // 0x33
    SCANCODE_MEDIA_PLAY_PAUSE,   // 0x34
    0,                           // 0x35
    0,                           // 0x36
    SCANCODE_ACPI_POWER,         // 0x37
    SCANCODE_MEDIA_WWW_BACK,     // 0x38
    0,                           // 0x39
    SCANCODE_MEDIA_WWW_HOME,     // 0x3a
    SCANCODE_MEDIA_PAUSE,        // 0x3b
    0,                           // 0x3c
    0,                           // 0x3d
    0,                           // 0x3e
    SCANCODE_ACPI_SLEEP,         // 0x3f
    
    SCANCODE_MEDIA_COMPUTER,     // 0x40
    0,                           // 0x41
    0,                           // 0x42
    0,                           // 0x43
    0,                           // 0x44
    0,                           // 0x45
    0,                           // 0x46
    0,                           // 0x47
    SCANCODE_MEDIA_EMAIL,        // 0x48
    0,                           // 0x49
    SCANCODE_KP_SLASH,           // 0x4a
    0,                           // 0x4b
    0,                           // 0x4c
    SCANCODE_MEDIA_NEXT_TRACK,   // 0x4d
    0,                           // 0x4e
    0,                           // 0x4f
    			
    SCANCODE_MEDIA_SELECT,       // 0x50
    0,                           // 0x51
    0,                           // 0x52
    0,                           // 0x53
    0,                           // 0x54
    0,                           // 0x55
    0,                           // 0x56
    0,                           // 0x57
    0,                           // 0x58
    0,                           // 0x59
    SCANCODE_KP_ENTER,           // 0x5a
    0,                           // 0x5b
    0,                           // 0x5c
    0,                           // 0x5d
    SCANCODE_ACPI_WAKE,          // 0x5e
    0,                           // 0x5f

    0,                           // 0x60
    0,                           // 0x61
    0,                           // 0x62
    0,                           // 0x63
    0,                           // 0x64
    0,                           // 0x65
    0,                           // 0x66
    0,                           // 0x67
    0,                           // 0x68
    SCANCODE_END,                // 0x69
    0,                           // 0x6a
    SCANCODE_LEFT,               // 0x6b
    SCANCODE_HOME,               // 0x6c
    0,                           // 0x6d
    0,                           // 0x6e
    0,                           // 0x6f
     					
    SCANCODE_INSERT,             // 0x70
    SCANCODE_DELETE,             // 0x71
    SCANCODE_DOWN,               // 0x72
    0,                           // 0x73
    SCANCODE_RIGHT,              // 0x74
    SCANCODE_UP,                 // 0x75
    0,                           // 0x76
    0,                           // 0x77
    0,                           // 0x78
    0,                           // 0x79
    SCANCODE_PAGE_DOWN,          // 0x7a
    0,                           // 0x7b
    SCANCODE_PRINTSCREEN,        // 0x7c
    SCANCODE_PAGE_UP,            // 0x7d
}; // uint8_t ps2_keyboard__set2_extended_scancode_array[]


const __flash uint8_t 
ps2_keyboard__set2_pause_scancode_sequence[8] = {
	0xe1, 0x14, 0x77, 0xe1, 0xf0, 0x14, 0xf0, 0x77
}; // ps2_keyboard__set2_pause_scancode_sequence

#define SCANCODE_MASK_KEY_RELEASE 0x8000


static uint8_t
ps2_keyboard__decode_set2_scancode(uint8_t code) {
	if (code >= sizeof(ps2_keyboard__set2_scancode_array))
		return 0x00;
		
	return ps2_keyboard__set2_scancode_array[code];
}


static uint8_t
ps2_keyboard__decode_set2_extended_scancode(uint8_t code) {
	if (code >= sizeof(ps2_keyboard__set2_extended_scancode_array))
		return 0x00;
		
	return ps2_keyboard__set2_extended_scancode_array[code];
}


static bool
ps2_keyboard__decode_set2_scancode_sequence(const __flash uint8_t* scancode_array, uint8_t len) {
	for( ; len != 0; --len, ++scancode_array)
		if (*scancode_array != ps2_keyboard__get_byte())
			return 0x00;
			
	return 0x01;
}

    							
enum ps2_keyboard_scancode_decoder_state {
    state_INIT,
    state_END,
    state_RELEASE_CODE,
    state_EXTENTED_CODE,
    state_EXTENTED_RELEASE_CODE
}; // enum ps2_keyboard_scancode_decoder_state


static uint16_t
ps2_keyboard__get_set2_scancode() {
	// This routine is for US QWERTY keyboards only
	// The code is written so that modifying the scancode tables should not take much effort
	uint16_t ret = 0x0000;
	
    const __flash uint8_t* scancode_array = ps2_keyboard__set2_scancode_array;
    uint8_t scancode_array_size = sizeof(ps2_keyboard__set2_scancode_array);
    
    uint8_t state = state_INIT;
    for(uint8_t state = state_INIT; state != state_END; ) {
    	uint8_t code = ps2_keyboard__get_byte();

    	switch(state) {
    		case state_INIT:
    			switch(code) {
    				case 0xe0:
    					state = state_EXTENTED_CODE;
    					break;
						
    				case 0xf0:
    					state = state_RELEASE_CODE;
    					break;
    					
    				default:
    					if (code == ps2_keyboard__set2_pause_scancode_sequence[0]) {
    						if (ps2_keyboard__decode_set2_scancode_sequence(ps2_keyboard__set2_pause_scancode_sequence + 1, sizeof(ps2_keyboard__set2_pause_scancode_sequence) - 1))
    							ret = SCANCODE_PAUSE;
    					}
    					else
	            			ret = ps2_keyboard__decode_set2_scancode(code);
	            			
	            		state = state_END;
    					break;
    			}
    		break; // case state_INIT
    		
    		case state_RELEASE_CODE:
	            ret = ps2_keyboard__decode_set2_scancode(code);
	            ret |= SCANCODE_MASK_KEY_RELEASE;
	            state = state_END;
    		break; // case state_RELEASE_CODE

			case state_EXTENTED_CODE:
				switch(code) {
    				case 0xf0:
    					state = state_EXTENTED_RELEASE_CODE;
    					break;

    				default:
	            		ret = ps2_keyboard__decode_set2_extended_scancode(code);	            		
	            		state = state_END;
    					break;    				
				}
			break; // case state_EXTENDED_CODE
			
    		case state_EXTENTED_RELEASE_CODE:
	            ret = ps2_keyboard__decode_set2_extended_scancode(code);
	            ret |= SCANCODE_MASK_KEY_RELEASE;
	            state = state_END;
    		break; // case state_EXTENDED_RELEASE_CODE	
    	}
    }
	
	return ret;
}


#endif /* AVRKIT_DRIVER_PS2_KEYBOARD_H */
