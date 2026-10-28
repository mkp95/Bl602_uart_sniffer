/*
 * network.c — TCP/UDP client for BL602 using lwip sockets.
 *
 * Connects to SERVER_IP:SERVER_PORT and forwards buffered data.
 * If USE_TCP is 0, a UDP socket is used instead (connectionless).
 */

#include "config.h"
#include "network.h"

#include <string.h>
#include <stdio.h>

#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"

/* -------------------------------------------------------------------------- */
/* State                                                                     */
/* -------------------------------------------------------------------------- */

static int g_sock = -1;
static struct sockaddr_in g_server_addr;
static bool g_connected = false;

/* -------------------------------------------------------------------------- */
/* Helpers                                                                   */
/* -------------------------------------------------------------------------- */

static int open_socket(void)
{
#if USE_TCP
    g_sock = socket(AF_INET, SOCK_STREAM, 0);
#else
    g_sock = socket(AF_INET, SOCK_DGRAM, 0);
#endif

    if (g_sock < 0) {
        return -1;
    }

    /* Set a send timeout so we don't block forever */
    int timeout_ms = 5000;
    lwip_setsockopt(g_sock, SOL_SOCKET, SO_SNDTIMEO,
                    &timeout_ms, sizeof(timeout_ms));

    return 0;
}

static int connect_socket(void)
{
    memset(&g_server_addr, 0, sizeof(g_server_addr));
    g_server_addr.sin_family = AF_INET;
    g_server_addr.sin_port = lwip_htons(SERVER_PORT);

    if (lwip_inet_aton(SERVER_IP, &g_server_addr.sin_addr) == 0) {
        return -1;
    }

#if USE_TCP
    if (lwip_connect(g_sock, (struct sockaddr *)&g_server_addr,
                     sizeof(g_server_addr)) != 0) {
        return -1;
    }
#endif

    return 0;
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                */
/* -------------------------------------------------------------------------- */

int net_init(void)
{
    if (g_connected) {
        return 0;
    }

    if (open_socket() != 0) {
        return -1;
    }

    if (connect_socket() != 0) {
        lwip_close(g_sock);
        g_sock = -1;
        return -2;
    }

    g_connected = true;
    return 0;
}

void net_deinit(void)
{
    if (g_sock >= 0) {
        lwip_close(g_sock);
        g_sock = -1;
    }
    g_connected = false;
}

int net_send(const uint8_t *buf, size_t len)
{
    if (!g_connected || g_sock < 0) {
        return -1;
    }

#if USE_TCP
    int sent = lwip_send(g_sock, buf, len, 0);
#else
    int sent = lwip_sendto(g_sock, buf, len, 0,
                           (struct sockaddr *)&g_server_addr,
                           sizeof(g_server_addr));
#endif

    if (sent < 0) {
        /* Connection likely dropped — mark for reconnect */
        g_connected = false;
        lwip_close(g_sock);
        g_sock = -1;
    }

    return sent;
}

bool net_is_connected(void)
{
    return g_connected;
}

int net_reconnect(void)
{
    net_deinit();

    /* Brief backoff before retrying */
    sys_msleep(1000);

    return net_init();
}
