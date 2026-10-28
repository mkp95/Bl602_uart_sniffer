#ifndef BL602_SDK_STUBS_H
#define BL602_SDK_STUBS_H

/*
 * sdk_stubs.h — Minimal type definitions for standalone compilation.
 *
 * These stubs allow the firmware to compile without the full bouffalo_sdk
 * installed. For production builds, replace these with the real SDK headers
 * (hal_uart.h, wifi_mgmr.h, lwip/sockets.h, FreeRTOS.h, etc.) from your
 * bouffalo_sdk checkout.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* --- UART HAL stubs --- */
typedef enum {
    HAL_UART_DATABITS_5  = 5,
    HAL_UART_DATABITS_6  = 6,
    HAL_UART_DATABITS_7  = 7,
    HAL_UART_DATABITS_8  = 8,
} hal_uart_databits_t;

typedef enum {
    HAL_UART_STOPBITS_1  = 1,
    HAL_UART_STOPBITS_2  = 2,
} hal_uart_stopbits_t;

typedef enum {
    HAL_UART_PARITY_NONE = 0,
    HAL_UART_PARITY_ODD  = 1,
    HAL_UART_PARITY_EVEN = 2,
} hal_uart_parity_t;

typedef enum {
    HAL_GPIO_PINMUX_UART = 1,
} hal_gpio_pinmux_t;

int hal_uart_init(uint8_t uart_id, uint32_t baud,
                  hal_uart_databits_t databits,
                  hal_uart_stopbits_t stopbits,
                  hal_uart_parity_t parity);

int hal_uart_deinit(uint8_t uart_id);
int hal_uart_read_byte(uint8_t uart_id);
int hal_uart_rx_available(uint8_t uart_id);
void hal_uart_clear_rx_irq(uint8_t uart_id);
int hal_uart_set_rx_irq_handler(uint8_t uart_id,
                                void (*handler)(uint8_t, void *),
                                void *arg);
int hal_uart_enable_rx_irq(uint8_t uart_id);
int hal_uart_disable_rx_irq(uint8_t uart_id);
int hal_gpio_pinmux_select(uint8_t pin, hal_gpio_pinmux_t func);

/* --- IRQ stubs --- */
int bl_irq_enable(int irqn);
int bl_irq_disable(int irqn);
int hal_uart_get_irqn(uint8_t uart_id);

/* --- WiFi manager stubs --- */
typedef struct {
    uint8_t ssid[32];
    uint8_t password[64];
} wifi_sta_config_t;

typedef enum {
    WIFI_MODE_STA = 1,
    WIFI_MODE_AP  = 2,
} wifi_mode_t;

typedef struct {
    uint32_t ip;
    uint32_t netmask;
    uint32_t gw;
} ip_info_t;

int wifi_mgmr_init(wifi_mode_t mode);
int wifi_mgmr_deinit(void);
int wifi_mgmr_register_event_cb(void (*cb)(uint32_t, void *), void *arg);
int wifi_mgmr_sta_connect(wifi_sta_config_t *cfg);
int wifi_mgmr_sta_disconnect(void);

#define WIFI_MGMR_EVENT_STA_CONNECTED   1
#define WIFI_MGMR_EVENT_STA_DISCONNECTED 2
#define WIFI_MGMR_EVENT_STA_GOT_IP      3

/* --- lwIP IP address stub --- */
typedef struct ip4_addr {
    uint32_t addr;
} ip4_addr_t;

#define ip4_addr_isany(a)   ((a)->addr == 0)
#define IP2STR(a)           (uint8_t *)(&(a)->addr)
#define IPSTR               "%d.%d.%d.%d"

/* --- FreeRTOS stubs --- */
#define pdMS_TO_TICKS(ms)   ((ms) / 10)
#define tskIDLE_PRIORITY    0

typedef struct TaskHandle_t *TaskHandle_t;

typedef int BaseType_t;
#define pdTRUE  1
#define pdFALSE 0

BaseType_t xTaskCreate(void (*func)(void *), const char *name,
                       uint32_t stack_words, void *param,
                       int prio, TaskHandle_t *handle);
void vTaskDelete(TaskHandle_t handle);
void vTaskDelay(uint32_t ticks);

#endif /* BL602_SDK_STUBS_H */
