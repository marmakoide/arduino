#ifndef AVRKIT_TYPES_H
#define AVRKIT_TYPES_H

#include <stdint.h>


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
STRUCT_NAME ## __push(volatile struct STRUCT_NAME * self, DATA_TYPE value) { \
    uint8_t next_end = (self->end + 1) % BUFFER_SIZE; \
    if (next_end != self->start) { \
	    self->data[self->end] = value; \
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

 
#endif /* AVRKIT_TYPES_H */
