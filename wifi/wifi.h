#ifndef BL602_WIFI_H
#define BL602_WIFI_H

#include <stdint.h>
#include <stdbool.h>

/*
 * WiFi connection module for BL602.
 * Connects to an AP and waits until an IP address is assigned.
 */

/* Initialize WiFi station mode and connect to the configured AP.
 * Returns 0 on success, negative on failure. */
int wifi_init(void);

/* Disconnect and stop WiFi. */
void wifi_deinit(void);

/* Block until connected and an IP address is acquired (or timeout_ms elapses).
 * Returns 0 when connected, -1 on timeout/failure. */
int wifi_wait_for_ip(uint32_t timeout_ms);

/* Return true if currently connected to an AP. */
bool wifi_is_connected(void);

/* Return the assigned IP address as a dotted-decimal string (caller provides buffer). */
int wifi_get_ip(char *buf, size_t len);

#endif /* BL602_WIFI_H */
