/*
 * uart.c — UART packet monitoring for BL602.
 *
 * Uses the Bouffalo Lab bouffalo_sdk UART driver. Reads incoming bytes
 * into a circular buffer so the network task can pull them out and
 * forward them to a remote server over WiFi.
 */

#include "config.h"
#include "uart.h"

#include <string.h>

/* SDK headers — using sdk_stubs.h for standalone compilation.
 * Replace with real bouffalo_sdk headers (hal_uart.h, hal_gpio.h, bl_irq.h)
 * for production builds targeting actual hardware. */
#include "sdk_stubs.h"

/* -------------------------------------------------------------------------- */
/* Circular RX buffer                                                        */
/* -------------------------------------------------------------------------- */

static uint8_t g_rx_buf[UART_RX_BUFFER_SIZE];
static volatile size_t g_rx_head = 0;   /* write index (ISR)          */
static volatile size_t g_rx_tail = 0;   /* read index  (task context) */

/* -------------------------------------------------------------------------- */
/* UART ISR                                                                  */
/* -------------------------------------------------------------------------- */

static void uart_irq_handler(uint8_t uart_id, void *arg)
{
    (void)arg;

    while (hal_uart_rx_available(uart_id)) {
        uint8_t ch = hal_uart_read_byte(uart_id);
        size_t next = (g_rx_head + 1) % UART_RX_BUFFER_SIZE;

        /* Drop byte if buffer is full (overrun) */
        if (next != g_rx_tail) {
            g_rx_buf[g_rx_head] = ch;
            g_rx_head = next;
        }
    }

    hal_uart_clear_rx_irq(uart_id);
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                */
/* -------------------------------------------------------------------------- */

int uart_init(void)
{
    /* Configure GPIO pins for UART function */
    hal_gpio_pinmux_select(UART_TX_PIN, HAL_GPIO_PINMUX_UART);
    hal_gpio_pinmux_select(UART_RX_PIN, HAL_GPIO_PINMUX_UART);

    /* Initialize UART peripheral */
    if (hal_uart_init(UART_ID, UART_BAUD, HAL_UART_DATABITS_8,
                      HAL_UART_STOPBITS_1, HAL_UART_PARITY_NONE) != 0) {
        return -1;
    }

    /* Enable RX interrupt */
    hal_uart_set_rx_irq_handler(UART_ID, uart_irq_handler, NULL);
    hal_uart_enable_rx_irq(UART_ID);

    bl_irq_enable(hal_uart_get_irqn(UART_ID));

    return 0;
}

void uart_deinit(void)
{
    hal_uart_disable_rx_irq(UART_ID);
    bl_irq_disable(hal_uart_get_irqn(UART_ID));
    hal_uart_deinit(UART_ID);

    g_rx_head = 0;
    g_rx_tail = 0;
}

int uart_read(uint8_t *buf, size_t len, uint32_t timeout_ms)
{
    (void)timeout_ms; /* blocking with timeout left as an exercise; non-blocking here */
    size_t copied = 0;

    while (copied < len) {
        size_t avail = (g_rx_head + UART_RX_BUFFER_SIZE - g_rx_tail) % UART_RX_BUFFER_SIZE;
        if (avail == 0) {
            break;
        }

        size_t to_copy = avail;
        if (copied + to_copy > len) {
            to_copy = len - copied;
        }

        /* Handle wrap-around in a single memcpy if possible */
        if (g_rx_tail + to_copy <= UART_RX_BUFFER_SIZE) {
            memcpy(buf + copied, g_rx_buf + g_rx_tail, to_copy);
            g_rx_tail = (g_rx_tail + to_copy) % UART_RX_BUFFER_SIZE;
        } else {
            size_t first = UART_RX_BUFFER_SIZE - g_rx_tail;
            memcpy(buf + copied, g_rx_buf + g_rx_tail, first);
            memcpy(buf + copied + first, g_rx_buf, to_copy - first);
            g_rx_tail = to_copy - first;
        }

        copied += to_copy;
    }

    return (int)copied;
}

size_t uart_available(void)
{
    return (g_rx_head + UART_RX_BUFFER_SIZE - g_rx_tail) % UART_RX_BUFFER_SIZE;
}

void uart_flush(void)
{
    g_rx_tail = g_rx_head;
}
