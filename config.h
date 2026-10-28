#ifndef BL602_CONFIG_H
#define BL602_CONFIG_H

/*
 * Configuration for BL602 UART-to-WiFi firmware.
 *
 * Edit these values to match your hardware and network before building.
 */

/* ---------- WiFi ---------- */
#define WIFI_SSID      "YOUR_SSID"
#define WIFI_PASSWORD  "YOUR_PASSWORD"

/* ---------- Server ---------- */
#define SERVER_IP   "192.168.1.100"
#define SERVER_PORT 8080

/* Use TCP (1) or UDP (0) */
#define USE_TCP 1

/* ---------- UART ---------- */
#define UART_ID     0      /* UART port index (0 or 1 on BL602) */
#define UART_BAUD   115200
#define UART_TX_PIN 16     /* GPIO pin for UART TX */
#define UART_RX_PIN 17     /* GPIO pin for UART RX */

/* ---------- Buffer / timing ---------- */
#define UART_RX_BUFFER_SIZE 1024
#define NETWORK_RECONNECT_MS 5000

#endif /* BL602_CONFIG_H */
