#include "logger.h"
#include "ring_buffer.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Ring buffer for storing log messages
static ring_buffer_t log_ring_buffer;
static uint8_t log_buffer[LOG_BUFFER_CAPACITY];

// Define a structure for a log entry
typedef struct {
    char format[LOG_ENTRY_MAX_LENGTH]; ///< Format string for the log
    uint8_t args[LOG_ENTRY_MAX_LENGTH]; ///< Serialized arguments
    size_t args_size; ///< Size of the serialized arguments
} log_entry_t;

/**
 * @brief Initialize the logger.
 *
 * Sets up the ring buffer for storing logs.
 */
void logger_init(void) {
    ring_buffer_init(&log_ring_buffer, log_buffer, sizeof(log_buffer));
}

/**
 * @brief Logs a formatted message.
 *
 * Adds a log entry to the ring buffer. If the buffer is full, older messages
 * are overwritten.
 *
 * @param format The format string (like printf).
 * @param ... Additional arguments for the format string.
 */
void log_message(const char *format, ...) {
    char temp_buffer[LOG_ENTRY_MAX_LENGTH];
    va_list args;

    // Format the message
    va_start(args, format);
    vsnprintf(temp_buffer, sizeof(temp_buffer), format, args);
    va_end(args);

    // Add to the ring buffer
    size_t message_length = strlen(temp_buffer);
    if (message_length > 0) {
        ring_buffer_enqueue(&log_ring_buffer, (const uint8_t *)temp_buffer, message_length);
        ring_buffer_enqueue(&log_ring_buffer, (const uint8_t *)"\r\n", 2); // Add newline
    }
}

/**
 * @brief Dump all logs to the console.
 */
void cmd_log_dump(const char *args) {
    if (ring_buffer_empty(&log_ring_buffer)) {
        console_write("Logger: No logs to display.\r\n");
        return;
    }

    console_write("Dumping logs:\r\n");
    char temp_buffer[LOG_ENTRY_MAX_LENGTH];
    while (!ring_buffer_empty(&log_ring_buffer)) {
        size_t len = ring_buffer_dequeue(&log_ring_buffer, (uint8_t *)temp_buffer, sizeof(temp_buffer) - 1);
        temp_buffer[len] = '\0'; // Null-terminate the string
        console_write(temp_buffer);
    }
}

/**
 * @brief Clear the log buffer.
 */
void cmd_log_clear(const char *args) {
    ring_buffer_init(&log_ring_buffer, log_buffer, sizeof(log_buffer));
    console_write("Logger: Log buffer cleared.\r\n");
}

