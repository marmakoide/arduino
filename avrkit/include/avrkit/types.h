#ifndef AVRKIT_TYPES_H
#define AVRKIT_TYPES_H

#include <stdint.h>

#include <avr/sleep.h>
#include <avr/interrupt.h>


/*
 * Boolean type
 */

typedef uint8_t bool;


/*
 * Generic static ring buffer type
 */

#define STATIC_RING_BUFFER_TYPE(STRUCT_NAME, DATA_TYPE, BUFFER_SIZE) \
struct STRUCT_NAME { \
    uint8_t start; \
    uint8_t end; \
    DATA_TYPE data[BUFFER_SIZE]; \
}; \
\
static volatile struct STRUCT_NAME \
STRUCT_NAME ## __singleton = { 0x00, 0x00 }; \
\
inline static void \
STRUCT_NAME ## __clear() { \
    STRUCT_NAME ## __singleton.start = STRUCT_NAME ## __singleton.end = 0; \
} \
\
inline static bool \
STRUCT_NAME ## __empty() { \
    return  STRUCT_NAME ## __singleton.start == STRUCT_NAME ## __singleton.end; \
} \
\
inline static bool \
STRUCT_NAME ## __full() { \
    uint8_t next_end = (STRUCT_NAME ## __singleton.end + 1) % BUFFER_SIZE; \
    return next_end == STRUCT_NAME ## __singleton.start; \
} \
\
inline static bool \
STRUCT_NAME ## __push(DATA_TYPE value) { \
    uint8_t next_end = (STRUCT_NAME ## __singleton.end + 1) % BUFFER_SIZE; \
    if (next_end != STRUCT_NAME ## __singleton.start) { \
	    STRUCT_NAME ## __singleton.data[STRUCT_NAME ## __singleton.end] = value; \
	    STRUCT_NAME ## __singleton.end = next_end; \
	    return 1; \
	} \
	return 0; \
} \
\
inline static void \
STRUCT_NAME ## __atomic_push_or_sleep(DATA_TYPE value) { \
    bool completed = 0; \
    do { \
        cli(); \
        bool full = STRUCT_NAME ## __full(); \
        if (!full) { \
            STRUCT_NAME ## __push(value); \
            completed = 1; \
        } \
        sei(); \
        \
        if (full) \
            sleep_mode(); \
    } while(!completed); \
} \
\
inline static DATA_TYPE \
STRUCT_NAME ## __pop() { \
    DATA_TYPE out = STRUCT_NAME ## __singleton.data[STRUCT_NAME ## __singleton.start]; \
    STRUCT_NAME ## __singleton.start = (STRUCT_NAME ## __singleton.start + 1) % BUFFER_SIZE; \
	return out; \
} \
\
inline static DATA_TYPE \
STRUCT_NAME ## __atomic_pop_or_sleep() { \
    DATA_TYPE ret; \
    bool completed = 0; \
    do { \
        cli(); \
        bool empty = STRUCT_NAME ## __empty(); \
        if (!empty) { \
            ret = STRUCT_NAME ## __pop(); \
	        completed = 1; \
	    } \
	    sei(); \
\
	    if (empty) \
    	    sleep_mode(); \
    } while(!completed); \
\
	return ret; \
}

 
#endif /* AVRKIT_TYPES_H */
