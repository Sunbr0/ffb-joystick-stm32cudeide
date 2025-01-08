/**
 * @file logger.h
 * @brief Logging library for embedded applications using the ring buffer and console libraries.
 *
 * This module provides functions for logging messages with different severity levels,
 * storing them in a ring buffer, and retrieving them via console commands.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include "ring_buffer.h"
#include "console.h"

// Log levels
typedef enum {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
} log_level_t;

// Logger configuration
#define LOG_BUFFER_CAPACITY 4096 ///< Ring buffer capacity for log storage
#define LOG_ENTRY_MAX_LENGTH 256 ///< Maximum length of a single log entry

// Function prototypes
void logger_init(void);
void log_message(const char *format, ...);
void cmd_log_clear(const char *args);
void cmd_log_dump(const char *args);

#endif // LOGGER_H
