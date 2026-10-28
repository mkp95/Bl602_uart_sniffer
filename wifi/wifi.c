/*
 * wifi.c — WiFi station connection for BL602.
 *
 * Uses the bouffalo_sdk WiFi management APIs. Connects to the AP
 * configured in config.h and waits for an IP via DHCP.
 */

#include "config.h"
#include "wifi.h"

#include <string.h>

/* SDK headers — using sdk_stubs.h for standalone compilation.
 * Replace with real bouffalo_sdk headers (wifi_mgmr.h, lwip/ip_addr.h, etc.)
 * for production builds targeting actual hardware. */
#include "sdk_stubs.h"

/* -------------------------------------------------------------------------- */
/* State                                                                     */
/* -------------------------------------------------------------------------- */

static bool g_wifi_connected = false;
static ip4_addr_t g_ip_addr;

/* Event callback from the WiFi manager */
static void wifi_event_cb(uint32_t event, void *param)
{
    (void)param;

    switch (event) {
        case WIFI_MGMR_EVENT_STA_CONNECTED:
            g_wifi_connected = true;
            break;

        case WIFI_MGMR_EVENT_STA_DISCONNECTED:
            g_wifi_connected = false;
            break;

        case WIFI_MGMR_EVENT_STA_GOT_IP:
            /* Extract IP from the event parameter if provided */
            if (param) {
                struct ip_info *info = (struct ip_info *)param;
                g_ip_addr = info->ip;
            }
            break;

        default:
            break;
    }
}

int wifi_init(void)
{
    /* Initialize WiFi manager in station mode */
    if (wifi_mgmr_init(WIFI_MODE_STA) != 0) {
        return -1;
    }

    /* Register event callbacks */
    wifi_mgmr_register_event_cb(wifi_event_cb, NULL);

    /* Configure station credentials */
    wifi_sta_config_t sta_cfg;
    memset(&sta_cfg, 0, sizeof(sta_cfg));
    strncpy((char *)sta_cfg.ssid, WIFI_SSID, sizeof(sta_cfg.ssid) - 1);
    strncpy((char *)sta_cfg.password, WIFI_PASSWORD, sizeof(sta_cfg.password) - 1);

    if (wifi_mgmr_sta_connect(&sta_cfg) != 0) {
        return -2;
    }

    return 0;
}

void wifi_deinit(void)
{
    wifi_mgmr_sta_disconnect();
    wifi_mgmr_deinit();
    g_wifi_connected = false;
}

int wifi_wait_for_ip(uint32_t timeout_ms)
{
    uint32_t elapsed = 0;
    uint32_t tick = 100; /* poll every 100 ms */

    while (elapsed < timeout_ms) {
        if (g_wifi_connected && !ip4_addr_isany(&g_ip_addr)) {
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(tick));
        elapsed += tick;
    }

    return -1;
}

bool wifi_is_connected(void)
{
    return g_wifi_connected;
}

int wifi_get_ip(char *buf, size_t len)
{
    if (!ip4_addr_isany(&g_ip_addr)) {
        snprintf(buf, len, IPSTR, IP2STR(&g_ip_addr));
        return 0;
    }
    return -1;
}
