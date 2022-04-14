#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define BAUD 9600 // Need to be defined before utils/setbaud.h inclusion
#include <util/setbaud.h>



extern const __flash uint8_t bitmap_data[512];


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

#define F_SCL 100000UL // Clock frequency for I2C protocol
//#define F_SCL 400000UL // Clock frequency for I2C protocol


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
}


void
twi_send_slave_address(uint8_t address) {
    TWDR = (address << 1) | TW_WRITE;
	TWCR = (1 << TWINT) | (1 << TWEN);
    loop_until_bit_is_set(TWCR, TWINT);	
}


void
twi_send_data(uint8_t data) {
    TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	loop_until_bit_is_set(TWCR, TWINT);
}



// --- SSD1306 handling -------------------------------------------------------

#define START_PAGE_ADDR           0
#define END_PAGE_ADDR             3
#define START_COLUMN_ADDR         0
#define END_COLUMN_ADDR           127
  
#define SSD1306_COMMAND           0x80  // Continuation bit=1, D/C=0; 1000 0000
#define SSD1306_COMMAND_STREAM    0x00  // Continuation bit=0, D/C=0; 0000 0000
#define SSD1306_DATA              0xc0  // Continuation bit=1, D/C=1; 1100 0000
#define SSD1306_DATA_STREAM       0x40  // Continuation bit=0, D/C=1; 0100 0000

#define SSD1306_SET_MUX_RATIO     0xa8
#define SSD1306_DISPLAY_OFFSET    0xd3
#define SSD1306_DISPLAY_ON        0xaf
#define SSD1306_DISPLAY_OFF       0xae
#define SSD1306_DIS_ENT_DISP_ON   0xa4
#define SSD1306_DIS_IGNORE_RAM    0xa5
#define SSD1306_DIS_NORMAL        0xa6
#define SSD1306_DIS_INVERSE       0xa7
#define SSD1306_DEACT_SCROLL      0x2e
#define SSD1306_ACTIVE_SCROLL     0x2f
#define SSD1306_SET_START_LINE    0x40
#define SSD1306_MEMORY_ADDR_MODE  0x20
#define SSD1306_SET_COLUMN_ADDR   0x21
#define SSD1306_SET_PAGE_ADDR     0x22
#define SSD1306_SEG_REMAP         0xa0
#define SSD1306_SEG_REMAP_OP      0xa1
#define SSD1306_COM_SCAN_DIR      0xc0
#define SSD1306_COM_SCAN_DIR_OP   0xc8
#define SSD1306_COM_PIN_CONF      0xda
#define SSD1306_SET_CONTRAST      0x81
#define SSD1306_SET_OSC_FREQ      0xd5
#define SSD1306_SET_CHAR_REG      0x8d
#define SSD1306_SET_PRECHARGE     0xd9
#define SSD1306_VCOM_DESELECT     0xdb

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2a  ///< Init diag scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xa3             ///< Set scroll range

static const uint8_t SSD1306_slave_address = 0x3c;


static const  __flash uint8_t 
SSD1306_init_sequence[] = {
  // number of initializers
  19,
  // 0xAE = Set Display OFF
  0, SSD1306_DISPLAY_OFF,
  // 0xA8
  1, SSD1306_SET_MUX_RATIO, 0x1f, // 0x3F,
  // 0x20 = Set Memory Addressing Mode
  // -----------------------------------
  // 0x00 - Horizontal Addressing Mode
  // 0x01 - Vertical Addressing Mode
  // 0x02 - Page Addressing Mode (RESET)
  1, SSD1306_MEMORY_ADDR_MODE, 0x00,
  // 0x21 = Set Column Address
  // -----------------------------------
  // 0x00 - Start Column Address
  // 0xFF - End Column Address
  2, SSD1306_SET_COLUMN_ADDR, START_COLUMN_ADDR, END_COLUMN_ADDR,
  // 0x22 = Set Page Address
  // -----------------------------------
  // 0x00 - Start Column Address
  // 0x07 - End Column Address
  2, SSD1306_SET_PAGE_ADDR, START_PAGE_ADDR, END_PAGE_ADDR,
  // 0x40
  0, SSD1306_SET_START_LINE,
  // 0xD3
  1, SSD1306_DISPLAY_OFFSET, 0x00,
  // 0xA0 / remap 0xA1
  0, SSD1306_SEG_REMAP_OP,
  // 0xC0 / remap 0xC8
  0, SSD1306_COM_SCAN_DIR_OP,
  // 0xDA
  1, SSD1306_COM_PIN_CONF, 0x02, //0x12,
  // 0x81
  1, SSD1306_SET_CONTRAST, 0x50,
  // 0xA4
  0, SSD1306_DIS_ENT_DISP_ON,
  // 0xA6
  0, SSD1306_DIS_NORMAL,
  // 0xD5
  1, SSD1306_SET_OSC_FREQ, 0x80,
  // 0xD9, 1st Period = higher value less blinking
  1, SSD1306_SET_PRECHARGE, 0xc2,
  // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
  1, SSD1306_VCOM_DESELECT, 0x20,
  // 0x8D
  1, SSD1306_SET_CHAR_REG, 0x14,
  // Deactivate scrolling
  0, SSD1306_DEACT_SCROLL,
  // 0xAF = Set Display ON
  0, SSD1306_DISPLAY_ON
}; // 


int
ssd1306_init() {
    // Send START
    fputs("twi start\r\n", &uart_output);
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    fputs("twi send slave address\r\n", &uart_output);
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;
    
    // Send SSD1306 startup sequence
    const __flash uint8_t* command_array_ptr = SSD1306_init_sequence;
    uint8_t command_count = *command_array_ptr++;
    for( ; command_count != 0; --command_count) {
        uint8_t arg_count = *command_array_ptr++;
        twi_send_data(SSD1306_COMMAND);
        twi_send_data(*command_array_ptr++);
        
        for( ;  arg_count != 0; --arg_count) {
            twi_send_data(SSD1306_COMMAND);
            twi_send_data(*command_array_ptr++);        
        }  
    }
    
    // Send stop
    fputs("twi stop\r\n", &uart_output);
    twi_stop();
    
    // Job done;
    return 1;
}


int
ssd1306_clear() {
   // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;
    
    // Send the bitmap data as a stream
    twi_send_data(SSD1306_DATA_STREAM);
    if (TW_STATUS != TW_MT_DATA_ACK)
        return 0;
    
    for(uint16_t i = 512; i != 0; --i) {
        twi_send_data(0x0);   
        if (TW_STATUS != TW_MT_DATA_ACK)
            return 0;         
    }
    
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;
}


int
ssd1306_upload_bitmap(const __flash uint8_t* bitmap) {
    // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;
    
    // Send the bitmap data as a stream
    twi_send_data(SSD1306_DATA_STREAM);
    if (TW_STATUS != TW_MT_DATA_ACK)
        return 0;
    
    const __flash uint8_t* pixel = bitmap;
    for(uint16_t i = 512; i != 0; --i, ++pixel) {
        twi_send_data(*pixel);
        if (TW_STATUS != TW_MT_DATA_ACK)
            return 0;
    }
    
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;
}


int
ssd1306_set_display_on() {
    // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;

    // Send command
    twi_send_data(SSD1306_COMMAND);
    twi_send_data(SSD1306_DISPLAY_ON);
        
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;    
}


int
ssd1306_set_display_off() {
    // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;

    // Send command
    twi_send_data(SSD1306_COMMAND);
    twi_send_data(SSD1306_DISPLAY_OFF);
        
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;    
}


int
ssd1306_set_normal_display_mode() {
    // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;

    // Send command
    twi_send_data(SSD1306_COMMAND);
    twi_send_data(SSD1306_DIS_NORMAL);
        
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;    
}


int
ssd1306_set_inverse_display_mode() {
    // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;

    // Send command
    twi_send_data(SSD1306_COMMAND);
    twi_send_data(SSD1306_DIS_INVERSE);
        
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;    
}


int
ssd1306_activate_scroll() {
   // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;

    // Send command
    twi_send_data(SSD1306_COMMAND);
    twi_send_data(SSD1306_ACTIVE_SCROLL);
        
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;    
}


int
ssd1306_deactivate_scroll() {
   // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;

    // Send command
    twi_send_data(SSD1306_COMMAND);
    twi_send_data(SSD1306_DEACT_SCROLL);
        
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;    
}


int
ssd1306_setup_horizontal_scroll(uint8_t start, uint8_t stop, int left_to_right) {
    twi_start();
    twi_send_slave_address(SSD1306_slave_address);
    
    twi_send_data(SSD1306_COMMAND_STREAM);
    
    if (left_to_right)
        twi_send_data(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    else
        twi_send_data(SSD1306_LEFT_HORIZONTAL_SCROLL);
    
    twi_send_data(0x00);    
    twi_send_data(start);
    twi_send_data(0x00);  
    twi_send_data(stop);
    twi_send_data(0x00);  
    twi_send_data(0xff);  
                                                             
    twi_stop();
    
    // Job done;
    return 1;    
}


int
ssd1306_set_vertical_offset(int8_t offset) {
   // Send START
    twi_start();
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return 0;        
    
    // Send slave address
    twi_send_slave_address(SSD1306_slave_address);
    if (TW_STATUS != TW_MT_SLA_ACK)
        return 0;

    // Send command
    twi_send_data(SSD1306_COMMAND);
    twi_send_data(SSD1306_DISPLAY_OFFSET);
    twi_send_data(offset);
    
    // Send stop
    twi_stop();
    
    // Job done;
    return 1;    
}


// --- Main entry point -------------------------------------------------------

int
main() {
	// Setup
	uart_init();
	twi_init();
	
	uint8_t ret = ssd1306_init();
	if (!ret) {
	    fputs("ssd1306 init failure\r\n", &uart_output);
	    goto waiting_loop;
    }
    
	// Upload the bitmap
	ret = ssd1306_upload_bitmap(bitmap_data);
	if (!ret) {
	    fputs("ssd1306 bitmap upload failure\r\n", &uart_output);
	}
	
    // Use several hardware function to animate the screen
    while(1) {
        // Flash the screen on and off
        for(uint8_t i = 0; i < 9; ++i) {
            if (i % 2)
                ssd1306_set_display_off();        
            else
                ssd1306_set_display_on();
            
            _delay_ms(10);
        }
    
        // Flash the screen with inverse and normal display mode
        for(uint8_t i = 0; i < 10; ++i) {
            if (i % 2)
                ssd1306_set_normal_display_mode();        
            else
                ssd1306_set_inverse_display_mode();
            
            _delay_ms(10);
        }
        
        // Trigger scrolling
        ssd1306_setup_horizontal_scroll(0, 3, 1);
        ssd1306_activate_scroll();
        _delay_ms(1000);
        ssd1306_deactivate_scroll(); 
        
        ssd1306_setup_horizontal_scroll(0, 3, 0);
        ssd1306_activate_scroll();
        _delay_ms(1000);
        ssd1306_deactivate_scroll();   
        
        
    }
    
	// Wait, do nothing loop
	waiting_loop:
	while(1) {
	    sleep_mode();
	}
}
