#ifndef BL602_UART_H
#define BL602_UART_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * UART packet monitoring module.
 * Reads bytes from the BL602 UART peripheral and makes them available
 * to the network forwarding task.
 */

/* Initialize the UART peripheral with config from config.h. Returns 0 on success. */
int uart_init(void);

/* Deinitialize the UART peripheral. */
void uart_deinit(void);

/*
 * Read up to `len` bytes from the UART RX buffer.
 * Returns the number of bytes read, or a negative error code.
 * Blocks for at most `timeout_ms` milliseconds.
 */
int uart_read(uint8_t *buf, size_t len, uint32_t timeout_ms);

/* Return the number of bytes currently available in the RX buffer. */
size_t uart_available(void);

/* Flush/discard any pending RX data. */
void uart_flush(void);

#endif /* BL602_UART_H */
