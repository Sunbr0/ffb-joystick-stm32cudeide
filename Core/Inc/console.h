/**
 * @file console.h
 * @brief Header file for the console system.
 *
 * The console handles user input via UART and executes registered commands.
 * It includes support for command parsing, input echoing, and output handling.
 *
 * Key Functions:
 * - `console_init()`: Initializes the console with the UART interface.
 * - `console_register_commands()`: Registers a list of commands for the console.
 * - `console_task()`: Processes input and executes commands.
 * - `console_write()`: Outputs text to the UART console.
 * - `console_uart_rx_callback()`: Handles UART reception events.
 *
 * Example:
 * ```c
 * UART_HandleTypeDef huart2;
 * console_init(&huart2);
 * console_register_commands(commands, sizeof(commands)/sizeof(commands[0]));
 * while (1) {
 *     console_task();
 * }
 * ```
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stddef.h>

// UART handle forward declaration
typedef struct __UART_HandleTypeDef UART_HandleTypeDef;

/**
 * @brief Command structure for the console.
 */
typedef struct {
    const char *name;      ///< Command name (e.g., "help").
    void (*handler)(const char *args); ///< Function to execute the command.
    const char *help;      ///< Description of the command.
} console_command_t;

// Function prototypes
void console_init(UART_HandleTypeDef *huart);
void console_register_commands(const void *cmds, size_t count);
void console_task(void);
void console_write(const char *str);
void cmd_clear(const char *args);

#endif // CONSOLE_H
