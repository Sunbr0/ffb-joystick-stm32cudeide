/**
 * @file ring_buffer.h
 * @brief Ring buffer (circular buffer) implementation for efficient data handling.
 *
 * This module provides a simple API for managing circular buffers, commonly
 * used in embedded systems for UART communication.
 *
 * Key Functions:
 * - `ring_buffer_init()`: Initializes the ring buffer.
 * - `ring_buffer_enqueue()`: Adds data to the buffer.
 * - `ring_buffer_dequeue()`: Removes data from the buffer.
 * - `ring_buffer_empty()` and `ring_buffer_full()`: Buffer state checks.
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    bool full;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, size_t capacity);
bool ring_buffer_empty(const ring_buffer_t *rb);
bool ring_buffer_full(const ring_buffer_t *rb);
size_t ring_buffer_size(const ring_buffer_t *rb);
static inline size_t ring_buffer_capacity(const ring_buffer_t *rb) {
    return rb->capacity;
}
size_t ring_buffer_enqueue(ring_buffer_t *rb, const uint8_t *data, size_t len);
size_t ring_buffer_dequeue(ring_buffer_t *rb, uint8_t *data, size_t len);

#endif // RING_BUFFER_H
