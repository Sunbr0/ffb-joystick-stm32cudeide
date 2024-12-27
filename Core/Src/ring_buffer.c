/**
 * @file ring_buffer.c
 * @brief Implementation of a simple circular buffer.
 *
 * This file provides the logic for managing a ring buffer, used for
 * UART input/output buffering in the console subsystem.
 *
 * Key Features:
 * - Efficient storage with a circular data structure.
 * - Handles wrap-around automatically.
 * - Supports enqueue and dequeue operations with bounds checking.
 */

// Key implementations:
// - `ring_buffer_enqueue`: Adds data to the buffer, handles wrap-around.
// - `ring_buffer_dequeue`: Reads data from the buffer.
// - Buffer state checks (`ring_buffer_empty`, `ring_buffer_full`).



#include "ring_buffer.h"

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, size_t capacity) {
    rb->buffer = buf;
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->full = false;
}

bool ring_buffer_empty(const ring_buffer_t *rb) {
    // Buffer is empty if head == tail and !full
    return (!rb->full && (rb->head == rb->tail));
}

bool ring_buffer_full(const ring_buffer_t *rb) {
    return rb->full;
}

size_t ring_buffer_size(const ring_buffer_t *rb) {
    if (!rb->full) {
        if (rb->tail >= rb->head) {
            return (rb->tail - rb->head);
        } else {
            return (rb->capacity - rb->head + rb->tail);
        }
    } else {
        return rb->capacity;
    }
}

size_t ring_buffer_enqueue(ring_buffer_t *rb, const uint8_t *data, size_t len) {
    if (len == 0) return 0;

    size_t free_space = rb->capacity - ring_buffer_size(rb);

    // If full, no space available
    if (free_space == 0) {
        return 0;
    }

    // Number of bytes we can actually write
    if (len > free_space) {
        len = free_space;
    }

    // Write data in two parts if it wraps around
    size_t space_to_end = rb->capacity - rb->tail;
    if (len > space_to_end) {
        // Write till end of buffer
        for (size_t i = 0; i < space_to_end; i++) {
            rb->buffer[rb->tail + i] = data[i];
        }
        // Wrap around
        size_t remaining = len - space_to_end;
        for (size_t i = 0; i < remaining; i++) {
            rb->buffer[i] = data[space_to_end + i];
        }
        rb->tail = remaining;
    } else {
        // Fits in one go
        for (size_t i = 0; i < len; i++) {
            rb->buffer[rb->tail + i] = data[i];
        }
        rb->tail = (rb->tail + len) % rb->capacity;
    }

    // Update full flag
    rb->full = (rb->tail == rb->head);

    return len;
}

size_t ring_buffer_dequeue(ring_buffer_t *rb, uint8_t *data, size_t len) {
    if (len == 0) return 0;

    size_t data_available = ring_buffer_size(rb);
    if (data_available == 0) {
        // Buffer empty
        return 0;
    }

    // Number of bytes we can actually read
    if (len > data_available) {
        len = data_available;
    }

    // Read data in two parts if it wraps
    size_t space_to_end = rb->capacity - rb->head;
    if (len > space_to_end) {
        // Read till end of buffer
        for (size_t i = 0; i < space_to_end; i++) {
            data[i] = rb->buffer[rb->head + i];
        }
        // Wrap around
        size_t remaining = len - space_to_end;
        for (size_t i = 0; i < remaining; i++) {
            data[space_to_end + i] = rb->buffer[i];
        }
        rb->head = remaining;
    } else {
        // Fits in one go
        for (size_t i = 0; i < len; i++) {
            data[i] = rb->buffer[rb->head + i];
        }
        rb->head = (rb->head + len) % rb->capacity;
    }

    rb->full = false; // Once we read, buffer cannot be full

    return len;
}
