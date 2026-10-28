#ifndef BL602_NETWORK_H
#define BL602_NETWORK_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Network client module for BL602.
 * Sends bytes over TCP (or UDP) to a remote server configured in config.h.
 */

/* Initialize the network client and connect to the server.
 * Returns 0 on success, negative on failure. */
int net_init(void);

/* Close the connection and deinitialize. */
void net_deinit(void);

/* Send `len` bytes to the server. Returns bytes sent, or negative on error. */
int net_send(const uint8_t *buf, size_t len);

/* Return true if the connection is currently open. */
bool net_is_connected(void);

/* Reconnect to the server (call periodically if the link drops). */
int net_reconnect(void);

#endif /* BL602_NETWORK_H */
