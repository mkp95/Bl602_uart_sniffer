/*
 * main.c — BL602 UART-to-WiFi firmware entry point.
 *
 * Initializes UART, WiFi, and the network client, then runs a forwarding
 * loop that reads packets from UART and sends them to the remote server.
 */

#include "config.h"
#include "uart.h"
#include "wifi.h"
#include "network.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

/* Forwarding task stack size (words) */
#define FWD_TASK_STACK_WORDS 1024
#define FWD_TASK_PRIORITY    (tskIDLE_PRIORITY + 3)

/* -------------------------------------------------------------------------- */
/* Forwarding task                                                            */
/* -------------------------------------------------------------------------- */

static void forwarding_task(void *pvParameters)
{
    (void)pvParameters;

    uint8_t buf[UART_RX_BUFFER_SIZE];

    for (;;) {
        /* Ensure WiFi is up */
        if (!wifi_is_connected()) {
            if (wifi_wait_for_ip(NETWORK_RECONNECT_MS) != 0) {
                printf("[main] WiFi not connected, retrying...\r\n");
                vTaskDelay(pdMS_TO_TICKS(NETWORK_RECONNECT_MS));
                continue;
            }
            printf("[main] WiFi connected\r\n");
        }

        /* Ensure network connection to server */
        if (!net_is_connected()) {
            if (net_init() != 0) {
                printf("[main] Failed to connect to server, retrying...\r\n");
                vTaskDelay(pdMS_TO_TICKS(NETWORK_RECONNECT_MS));
                continue;
            }
            printf("[main] Connected to server %s:%d\r\n", SERVER_IP, SERVER_PORT);
        }

        /* Drain UART -> network */
        int n = uart_read(buf, sizeof(buf), 0);
        if (n > 0) {
            int sent = net_send(buf, (size_t)n);
            if (sent < 0) {
                printf("[main] Send failed, will reconnect\r\n");
                net_reconnect();
            } else {
                /* Optional: echo bytes back to UART for debugging */
                // uart_write(buf, (size_t)sent);
            }
        }

        /* Yield so other tasks run when there is no data */
        if (n <= 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

/* -------------------------------------------------------------------------- */
/* app_main — entry point called by the BL602 SDK                            */
/* -------------------------------------------------------------------------- */

void app_main(void)
{
    printf("\r\n========================================\r\n");
    printf(" BL602 UART-to-WiFi Forwarder\r\n");
    printf("========================================\r\n");

    /* Initialize UART monitoring */
    if (uart_init() != 0) {
        printf("[main] ERROR: UART init failed\r\n");
        while (1) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }
    printf("[main] UART initialized (baud %d)\r\n", UART_BAUD);

    /* Initialize WiFi */
    if (wifi_init() != 0) {
        printf("[main] ERROR: WiFi init failed\r\n");
        while (1) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }
    printf("[main] WiFi connecting to %s...\r\n", WIFI_SSID);

    /* Wait for IP */
    if (wifi_wait_for_ip(30000) != 0) {
        printf("[main] ERROR: No IP acquired\r\n");
        while (1) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }

    char ip_str[16];
    wifi_get_ip(ip_str, sizeof(ip_str));
    printf("[main] IP address: %s\r\n", ip_str);

    /* Create the forwarding task */
    xTaskCreate(forwarding_task, "fwd_task", FWD_TASK_STACK_WORDS,
                NULL, FWD_TASK_PRIORITY, NULL);

    /* app_main task can delete itself or idle */
    vTaskDelete(NULL);
}
