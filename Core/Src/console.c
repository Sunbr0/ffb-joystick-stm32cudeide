/**
 * @file console.c
 * @brief Implementation of the UART console.
 *
 * This file handles:
 * - Receiving user input through a ring buffer.
 * - Parsing and executing registered commands.
 * - Sending output back to the user.
 *
 * Key Features:
 * - Line editing (backspace support).
 * - Command registration and lookup.
 * - Output buffering for efficient UART transmission.
 */

// Highlights:
// - `console_task()`: Processes incoming characters and executes commands.
// - `console_write()`: Queues output data for UART transmission.
// - `cmd_clear()`: Example command to clear the console screen.


#include "console.h"
#include "ring_buffer.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

#define CONSOLE_MAX_LINE_LENGTH 256
#define INPUT_BUFFER_SIZE 256
#define OUTPUT_BUFFER_SIZE 512

// UART handle
static UART_HandleTypeDef *g_huart = NULL;

// Ring buffers and their storage placed here instead of main.c
static uint8_t input_storage[INPUT_BUFFER_SIZE];
static uint8_t output_storage[OUTPUT_BUFFER_SIZE];
static ring_buffer_t input_rb;
static ring_buffer_t output_rb;

// A flag to indicate if a TX is ongoing
static volatile bool tx_in_progress = false;

// Line handling
static char line_buffer[CONSOLE_MAX_LINE_LENGTH];
static size_t line_pos = 0;

// Commands (stored by register function)
static const void *g_commands = NULL;
static size_t g_command_count = 0;

static uint8_t rx_char;

// Forward declarations
static void console_process_line(char *line);
static void console_send_char(uint8_t c);
static void console_start_tx(void);





void console_init(UART_HandleTypeDef *huart)
{
    g_huart = huart;

    // Initialize ring buffers
    ring_buffer_init(&input_rb, input_storage, sizeof(input_storage));
    ring_buffer_init(&output_rb, output_storage, sizeof(output_storage));

    tx_in_progress = false;

    // Start receiving characters
    HAL_UART_Receive_IT(g_huart, &rx_char, 1);

    // Print welcome message and prompt
    console_write("\r\nConsole Ready\r\n> ");
    line_pos = 0;
}

void console_register_commands(const void *cmds, size_t count)
{
    g_commands = cmds;
    g_command_count = count;
}

void console_write(const char *str)
{
    size_t len = strlen(str);
    ring_buffer_enqueue(&output_rb, (const uint8_t*)str, len);

    // If no transmission is in progress, start one now
    if (!tx_in_progress) {
        console_start_tx();
    }
}

void console_clear_screen(void)
{
    console_write("\033[2J\033[H");
}

void cmd_clear(const char *args)
{
    (void)args; // Ignore arguments for the clear command
    console_clear_screen();
    console_write("> "); // Reprint the prompt
}


static void console_start_tx(void)
{
    if (tx_in_progress) {
        return; // Already transmitting
    }

    size_t available = ring_buffer_size(&output_rb);
    if (available == 0) {
        // Nothing to send
        return;
    }

    // Copy data from ring buffer into a temporary buffer for transmission
    // (If data is contiguous, you could transmit directly. Here we show a safe approach.)
    static uint8_t tx_buffer[64]; // adjust size as needed
    if (available > sizeof(tx_buffer)) {
        available = sizeof(tx_buffer); // send up to tx_buffer size
    }

    ring_buffer_dequeue(&output_rb, tx_buffer, available);

    // Start transmit in interrupt mode
    if (HAL_UART_Transmit_IT(g_huart, tx_buffer, available) == HAL_OK) {
        tx_in_progress = true;
    }
}


static void console_send_char(uint8_t c)
{
    HAL_UART_Transmit(g_huart, &c, 1, HAL_MAX_DELAY);
}

void console_task(void)
{
    while (!ring_buffer_empty(&input_rb)) {
        uint8_t c;

        if (ring_buffer_dequeue(&input_rb, &c, 1) == 1) {


            // Filter out unwanted non-printable characters except backspace, newline
            if ((c < 0x20 && c != '\r' && c != '\n' && c != 0x08 && c != 0x7F)) {
                continue; // Ignore invalid characters
            }

            if (c == 0x08 || c == 0x7F) { // Backspace handling
                if (line_pos > 0) {
                    line_pos--;              // Remove the last character
                    console_write("\b \b"); // Visually erase it
                }
                continue; // Skip further processing for backspace
            }

            // Echo valid characters so the user sees them
            console_send_char(c);

            if (c == '\r' || c == '\n') {
                // End of line, process command
                line_buffer[line_pos] = '\0'; // Null-terminate the command
                console_process_line(line_buffer); // Process command
                console_write("\r> "); // Print new prompt
                line_pos = 0; // Reset line position
            } else {
                // Add character to the command buffer
                if (line_pos < (CONSOLE_MAX_LINE_LENGTH - 1)) {
                    line_buffer[line_pos++] = (char)c;
                } else {
                    // Line too long, notify user and reset
                    console_write("\r\nError: line too long\r\n> ");
                    line_pos = 0;
                }
            }
        }
    }
}

static void console_process_line(char *line)
{
    // Trim newline
    char *end = line + strlen(line) - 1;
    while (end > line && (*end == '\r' || *end == '\n')) {
        *end = '\0';
        end--;
    }

    if (strlen(line) == 0) {
        console_write("> ");
        line_pos = 0; // Reset here to avoid carrying over previous values
        return;
    }

    if (strcmp(line, "clear") == 0) {
		cmd_clear(NULL); // Call clear command, which handles its own prompt
		return;          // Avoid printing another prompt
	}

    // Parse command and args
    char *args = line;
    char *cmd = strsep(&args, " ");

    // Cast to command array type
    const console_command_t *cmds = (const console_command_t *)g_commands;

    for (size_t i = 0; i < g_command_count; i++) {
        if (strcmp(cmds[i].name, cmd) == 0) {
            cmds[i].handler(args);
            console_write("> ");
            return;
        }
    }

    console_write("Unknown command: ");
    console_write(cmd);
    console_write("\r\n> ");
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == g_huart)
    {
    	ring_buffer_enqueue(&input_rb, &rx_char, 1);
    	HAL_UART_Receive_IT(g_huart, &rx_char, 1);
    }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == g_huart) {
        // Transmission done
        tx_in_progress = false;

        // If more data in the output buffer, start next transmit
        if (ring_buffer_size(&output_rb) > 0) {
            console_start_tx();
        }
    }
}

