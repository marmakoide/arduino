#ifndef AVRKIT_USART_H
#define AVRKIT_USART_H

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include <avrkit/types.h>

    
    
/*
 * USART state observation routines
 */

#define USARTx__IS_RX_COMPLETE(USART_ID) \
inline static bool \
usart ## USART_ID ## __is_rx_complete() { \
    return UCSR ## USART_ID ## A & (1 << RXC ## USART_ID ) != 0; \
}

#define USARTx__IS_TX_COMPLETE(USART_ID) \
inline static bool \
usart ## USART_ID ## __is_tx_complete() { \
    return UCSR ## USART_ID ## A & (1 << TXC ## USART_ID ) != 0; \
 }

#define USARTx__IS_DATA_REGISTER_EMPTY(USART_ID) \
inline static bool \
usart ## USART_ID ## __is_data_register_empty() { \
    return UCSR ## USART_ID ## A & (1 << UDRE ## USART_ID ) != 0; \
}

#define USARTx__HAS_FRAME_ERROR(USART_ID) \
inline static bool \
usart ## USART_ID ## __has_frame_error() { \
    return UCSR ## USART_ID ## A & (1 << FE ## USART_ID ) != 0; \
}

#define USARTx__HAS_DATA_OVERRUN(USART_ID) \
inline static bool \
usart ## USART_ID ## __has_data_overrun() { \
    return UCSR ## USART_ID ## A & (1 << DOR ## USART_ID ) != 0; \
}

#define USARTx__HAS_PARITY_ERROR(USART_ID) \
inline static bool \
usart ## USART_ID ## __has_parity_error() { \
    return UCSR ## USART_ID ## A & (1 << UPE ## USART_ID ) != 0; \
}


/*
 * USART state setup routines
 */

#define USARTx__SET_MODE_ASYNCHRONOUS(USART_ID) \
inline static void \
usart ## USART_ID ## __set_mode_asynchronous() { \
    UCSR ## USART_ID ## C &= ~(1 << UMSEL ## USART_ID ## 1 ); \
    UCSR ## USART_ID ## C &= ~(1 << UMSEL ## USART_ID ## 0 ); \
    UCSR ## USART_ID ## C &= ~(1 << UCPOL ## USART_ID ); \
}

#define USARTx__SET_MODE_SYNCHRONOUS(USART_ID) \
inline static void \
usart ## USART_ID ## __set_mode_synchronous() { \
    UCSR ## USART_ID ## C &= ~(1 << UMSEL ## USART_ID ## 1 ); \
    UCSR ## USART_ID ## C |= 1 << UMSEL ## USART_ID ## 0; \
}

#define USARTx__SET_MODE_MASTER_SPI(USART_ID) \
inline static void \
usart ## USART_ID ## __set_mode_master_spi() { \
    UCSR ## USART_ID ## C |= 1 << UMSEL ## USART_ID ## 1; \
    UCSR ## USART_ID ## C |= 1 << UMSEL ## USART_ID ## 0; \
}

#define USARTx__SET_CHAR_SIZE_5(USART_ID) \
inline static void \
usart ## USART_ID ## __set_char_size_5() { \
    UCSR ## USART_ID ## B &= ~(1 << UCSZ ## USART_ID ## 2); \
    UCSR ## USART_ID ## C &= ~(1 << UCSZ ## USART_ID ## 1); \
    UCSR ## USART_ID ## C &= ~(1 << UCSZ ## USART_ID ## 0); \
}

#define USARTx__SET_CHAR_SIZE_6(USART_ID) \
inline static void \
usart ## USART_ID ## __set_char_size_6() { \
    UCSR ## USART_ID ## B &= ~(1 << UCSZ ## USART_ID ## 2); \
    UCSR ## USART_ID ## C &= ~(1 << UCSZ ## USART_ID ## 1); \
    UCSR ## USART_ID ## C |= 1 << UCSZ ## USART_ID ## 0 ; \
}

#define USARTx__SET_CHAR_SIZE_7(USART_ID) \
inline static void \
usart ## USART_ID ## __set_char_size_7() { \
    UCSR ## USART_ID ## B &= ~(1 << UCSZ ## USART_ID ## 2); \
    UCSR ## USART_ID ## C |= 1 << UCSZ ## USART_ID ## 1 ; \
    UCSR ## USART_ID ## C &= ~(1 << UCSZ ## USART_ID ## 0 ); \
}

#define USARTx__SET_CHAR_SIZE_8(USART_ID) \
inline static void \
usart ## USART_ID ## __set_char_size_8() { \
    UCSR ## USART_ID ## B &= ~(1 << UCSZ ## USART_ID ## 2 ); \
    UCSR ## USART_ID ## C |= 1 << UCSZ ## USART_ID ## 1 ; \
    UCSR ## USART_ID ## C |= 1 << UCSZ ## USART_ID ## 0; \
}

#define USARTx__SET_CHAR_SIZE_9(USART_ID) \
inline static void \
usart ## USART_ID ## __set_char_size_9() { \
    UCSR ## USART_ID ## B |= 1 << UCSZ ## USART_ID ## 2 ; \
    UCSR ## USART_ID ## C |= 1 << UCSZ ## USART_ID ## 1 ; \
    UCSR ## USART_ID ## C |= 1 << UCSZ ## USART_ID ## 0 ; \
}

#define USARTx__SET_STOP_BITS_1(USART_ID) \
inline static void \
usart ## USART_ID ## __set_stop_bits_1() { \
    UCSR ## USART_ID ## C &= ~(1 << USBS ## USART_ID ); \
}

#define USARTx__SET_STOP_BITS_2(USART_ID) \
inline static void \
usart ## USART_ID ## __set_stop_bits_2() { \
    UCSR ## USART_ID ## C |= 1 << USBS ## USART_ID ; \
}

#define USARTx__SET_PARITY_NONE(USART_ID) \
inline static void \
usart ## USART_ID ## __set_parity_none() { \
    UCSR ## USART_ID ## C &= ~(1 << UPM ## USART_ID ## 0 ); \
    UCSR ## USART_ID ## C &= ~(1 << UPM ## USART_ID ## 1 ); \
}

#define USARTx__SET_PARITY_EVEN(USART_ID) \
inline static void \
usart ## USART_ID ## __set_parity_even() { \
    UCSR ## USART_ID ## C |= (1 << UPM ## USART_ID ## 0 ); \
    UCSR ## USART_ID ## C &= ~(1 << UPM ## USART_ID ## 1 ); \
}

#define USARTx__SET_PARITY_ODD(USART_ID) \
inline static void \
usart ## USART_ID ## __set_parity_odd() { \
    UCSR ## USART_ID ## C |= (1 << UPM ## USART_ID ## 0 ); \
    UCSR ## USART_ID ## C |= (1 << UPM ## USART_ID ## 1 ); \
}

#define USARTx__SET_BAUDS(USART_ID, TOL_PERCENT) \
inline static void \
usart ## USART_ID ## __set_bauds(unsigned long bauds) { \
    unsigned long UBRR_value = (((F_CPU) + 8UL * (bauds)) / (16UL * (bauds)) -1UL); \
    bool use_2x = 100 * (F_CPU) > (16 * ((UBRR_value) + 1)) * (100 * (bauds) + (bauds) * (TOL_PERCENT)); \
\
    unsigned long UBRR_value_adjusted = use_2x ? (((F_CPU) + 4UL * (bauds)) / (8UL * (bauds)) -1UL) : UBRR_value; \
\
    UBRR ## USART_ID ## H = UBRR_value_adjusted >> 8; \
    UBRR ## USART_ID ## L = UBRR_value_adjusted & 0xff; \
\
    if (use_2x) \
        UCSR ## USART_ID ## A |= 1 << U2X ## USART_ID ; \
    else \
        UCSR ## USART_ID ## A &= ~(1 << U2X ## USART_ID ); \
}


/*
 * Methods for USART interrupts handling
 */

#define USARTx__ENABLE_RX_COMPLETE_INTERRUPT(USART_ID) \
inline static void \
usart ## USART_ID ## __enable_rx_complete_interrupt() { \
    UCSR ## USART_ID ## B |= 1 << RXCIE ## USART_ID ; \
}

#define USARTx__DISABLE_RX_COMPLETE_INTERRUPT(USART_ID) \
inline static void \
usart ## USART_ID ## __disable_rx_complete_interrupt() { \
    UCSR ## USART_ID ## B |= ~(1 << RXCIE ## USART_ID ); \
}

#define USARTx__ENABLE_TX_COMPLETE_INTERRUPT(USART_ID) \
inline static void \
usart ## USART_ID ## __enable_tx_complete_interrupt() { \
    UCSR ## USART_ID ## B |= 1 << TXCIE ## USART_ID ; \
}

#define USARTx__DISABLE_TX_COMPLETE_INTERRUPT(USART_ID) \
inline static void \
usart ## USART_ID ## __disable_tx_complete_interrupt() { \
    UCSR ## USART_ID ## B |= ~(1 << TXCIE ## USART_ID ); \
}

#define USARTx__ENABLE_DATA_REGISTER_EMPTY_INTERRUPT(USART_ID) \
inline static void \
usart ## USART_ID ## __enable_data_register_empty_interrupt() { \
    UCSR ## USART_ID ## B |= 1 << UDRIE ## USART_ID ; \
}

#define USARTx__DISABLE_DATA_REGISTER_EMPTY_INTERRUPT(USART_ID) \
inline static void \
usart ## USART_ID ## __disable_data_register_empty_interrupt() { \
    UCSR ## USART_ID ## B |= ~(1 << UDRIE ## USART_ID ); \
}

#define USARTx__ENABLE_TX(USART_ID) \
inline static void \
usart ## USART_ID ## __enable_tx() { \
    UCSR ## USART_ID ## B |= 1 << TXEN ## USART_ID ; \
}

#define USARTx__DISABLE_TX(USART_ID) \
inline static void \
usart ## USART_ID ## __disable_tx() { \
    UCSR ## USART_ID ## B |= ~(1 << TXEN ## USART_ID ); \
}

#define USARTx__ENABLE_RX(USART_ID) \
inline static void \
usart ## USART_ID ## __enable_rx() { \
    UCSR ## USART_ID ## B |= 1 << RXEN ## USART_ID ; \
}

#define USARTx__DISABLE_RX(USART_ID) \
inline static void \
usart ## USART_ID ## __disable_rx() { \
    UCSR ## USART_ID ## B |= ~(1 << RXEN ## USART_ID ); \
}

     
/*
 * Instanciate all the USART definitions
 */

#define INSTANCIATE_USART_DEFINITIONS(USART_ID) \
  USARTx__IS_RX_COMPLETE(USART_ID) \
  USARTx__IS_TX_COMPLETE(USART_ID) \
  USARTx__IS_DATA_REGISTER_EMPTY(USART_ID) \
  USARTx__HAS_FRAME_ERROR(USART_ID) \
  USARTx__HAS_DATA_OVERRUN(USART_ID) \
  USARTx__HAS_PARITY_ERROR(USART_ID) \
  USARTx__SET_MODE_ASYNCHRONOUS(USART_ID) \
  USARTx__SET_MODE_SYNCHRONOUS(USART_ID) \
  USARTx__SET_MODE_MASTER_SPI(USART_ID) \
  USARTx__SET_CHAR_SIZE_5(USART_ID) \
  USARTx__SET_CHAR_SIZE_6(USART_ID) \
  USARTx__SET_CHAR_SIZE_7(USART_ID) \
  USARTx__SET_CHAR_SIZE_8(USART_ID) \
  USARTx__SET_CHAR_SIZE_9(USART_ID) \
  USARTx__SET_STOP_BITS_1(USART_ID) \
  USARTx__SET_STOP_BITS_2(USART_ID) \
  USARTx__SET_PARITY_NONE(USART_ID) \
  USARTx__SET_PARITY_EVEN(USART_ID) \
  USARTx__SET_PARITY_ODD(USART_ID) \
  USARTx__SET_BAUDS(USART_ID, 2) \
  USARTx__ENABLE_RX_COMPLETE_INTERRUPT(USART_ID) \
  USARTx__DISABLE_RX_COMPLETE_INTERRUPT(USART_ID) \
  USARTx__ENABLE_TX_COMPLETE_INTERRUPT(USART_ID) \
  USARTx__DISABLE_TX_COMPLETE_INTERRUPT(USART_ID) \
  USARTx__ENABLE_DATA_REGISTER_EMPTY_INTERRUPT(USART_ID) \
  USARTx__DISABLE_DATA_REGISTER_EMPTY_INTERRUPT(USART_ID) \
  USARTx__ENABLE_TX(USART_ID) \
  USARTx__DISABLE_TX(USART_ID) \
  USARTx__ENABLE_RX(USART_ID) \
  USARTx__DISABLE_RX(USART_ID)

INSTANCIATE_USART_DEFINITIONS(0)


/*
 * USART ring buffer serviced by interrupts
 */

#define DEFAULT_USART_RING_BUFFER_SIZE 16


// Generic ring buffer defintion
#define USART_RING_BUFFER_TYPE(STRUCT_NAME, DATA_TYPE, BUFFER_SIZE) \
struct STRUCT_NAME { \
    uint8_t start; \
    uint8_t end; \
    DATA_TYPE data[BUFFER_SIZE]; \
}; \
\
inline static void \
STRUCT_NAME ## __clear(volatile struct STRUCT_NAME * self) { \
    self->start = self->end = 0; \
} \
\
inline static bool \
STRUCT_NAME ## __empty(volatile const struct STRUCT_NAME * self) { \
    return self->start == self->end; \
} \
\
inline static bool \
STRUCT_NAME ## __full(volatile const struct STRUCT_NAME * self) { \
    uint8_t next_end = (self->end + 1) % BUFFER_SIZE; \
    return next_end == self->start; \
} \
\
inline static bool \
STRUCT_NAME ## __push(volatile struct STRUCT_NAME * self, DATA_TYPE c) { \
    uint8_t next_end = (self->end + 1) % BUFFER_SIZE; \
    if (next_end != self->start) { \
	    self->data[self->end] = c; \
	    self->end = next_end; \
	    return 1; \
	} \
	return 0; \
} \
\
inline static DATA_TYPE \
STRUCT_NAME ## __pop(volatile struct STRUCT_NAME * self) { \
    DATA_TYPE out = self->data[self->start]; \
    self->start = (self->start + 1) % BUFFER_SIZE; \
	return out; \
}


// Transmission ring buffer management
#define USART_TX_BUFFER(USART_ID, STRUCT_NAME, BUFFER_SIZE) \
USART_RING_BUFFER_TYPE(STRUCT_NAME, char, BUFFER_SIZE) \
\
static volatile struct STRUCT_NAME \
STRUCT_NAME ## __singleton = { 0x00, 0x00 };\
\
inline static void \
usart ## USART_ID ## __on_data_register_empty() { \
  if (!STRUCT_NAME ## __empty(& STRUCT_NAME ## __singleton)) \
    UDR ## USART_ID = STRUCT_NAME ## __pop(& STRUCT_NAME ## __singleton); \
} \
\
static void \
usart ## USART_ID ## __send_char(char c) { \
    bool completed = 0; \
    do { \
        cli(); \
        bool full = STRUCT_NAME ## __full(& STRUCT_NAME ## __singleton); \
        if (!full) { \
            STRUCT_NAME ## __push(& STRUCT_NAME ## __singleton, c); \
            completed = 1; \
        } \
        sei(); \
        \
        if (full) \
            sleep_mode(); \
    } while(!completed); \
} \
\
static void \
usart ## USART_ID ## __send_str(const char* str) { \
    for( ; *str != '\0'; ++str) \
        usart ## USART_ID ## __send_char(*str); \
}


// Instanciation of the transmission ring buffer
#ifndef USART0_TX_BUFFER_SIZE
  #define USART0_TX_BUFFER_SIZE DEFAULT_USART_RING_BUFFER_SIZE
#endif

#define INSTANCIATE_USART_TX_BUFFER(USART_ID) \
  USART_TX_BUFFER(USART_ID, usart ## USART_ID ## __TxRingBuffer, USART ## USART_ID ## _TX_BUFFER_SIZE)

#ifdef ENABLE_USART0_TX_BUFFER
  INSTANCIATE_USART_TX_BUFFER(0)  
#endif


// Reception ring buffer management
#define USART_RX_BUFFER(USART_ID, STRUCT_NAME, BUFFER_SIZE) \
USART_RING_BUFFER_TYPE(STRUCT_NAME, char, BUFFER_SIZE) \
\
static volatile struct STRUCT_NAME \
STRUCT_NAME ## __singleton = { 0x00, 0x00 };\
\
inline static void  \
usart ## USART_ID ##  __on_rx_complete() { \
    if (! STRUCT_NAME ## __full(& STRUCT_NAME ## __singleton)) \
        STRUCT_NAME ## __push(& STRUCT_NAME ## __singleton, UDR ## USART_ID ); \
} \
\
static char \
usart ## USART_ID ## __get_char() { \
    char ret; \
    bool completed = 0; \
    do { \
        cli(); \
        bool empty = STRUCT_NAME ## __empty(& STRUCT_NAME ## __singleton ); \
        if (!empty) { \
            ret = STRUCT_NAME ## __pop(& STRUCT_NAME ## __singleton ); \
	        completed = 1; \
	    } \
	    sei(); \
\
	    if (empty) \
    	    sleep_mode(); \
    } while(!completed); \
\
	return ret; \
} \
\
static uint8_t \
usart ## USART_ID ## __get_line(char* str, uint8_t len) { \
    uint8_t i = 0; \
    for( ; i < len - 1; ++i, ++str) { \
        *str = usart ## USART_ID ## __get_char(); \
        if (*str == '\n') \
            break; \
    } \
\
    *str = '\0'; \
    return i; \
}

// Instanciation of the transmission ring buffer
#ifndef USART0_RX_BUFFER_SIZE
  #define USART0_RX_BUFFER_SIZE DEFAULT_USART_RING_BUFFER_SIZE
#endif

#define INSTANCIATE_USART_RX_BUFFER(USART_ID) \
  USART_RX_BUFFER(USART_ID, usart ## USART_ID ## __RxRingBuffer, USART ## USART_ID ## _RX_BUFFER_SIZE)
    
#ifdef ENABLE_USART0_RX_BUFFER
  INSTANCIATE_USART_RX_BUFFER(0)  
#endif


#endif /* AVRKIT_USART_H */
    
