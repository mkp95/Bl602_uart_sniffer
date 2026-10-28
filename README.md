# BL602 UART-to-WiFi Firmware

Firmware for the Bouffalo Lab **BL602** (RISC-V + WiFi) that monitors UART
packets and forwards them to a remote server over WiFi (TCP or UDP).

## Project structure

```
bl602_uart_wifi_fw/
├── config.h            # WiFi credentials, server IP/port, UART pins/baud
├── CMakeLists.txt      # Top-level build (requires bouffalo_sdk)
├── README.md
├── .github/workflows/build.yml   # GitHub Actions build
├── main/
│   ├── CMakeLists.txt
│   └── main.c          # Entry point + forwarding task
├── uart/
│   ├── CMakeLists.txt
│   ├── uart.h
│   └── uart.c          # UART RX ring buffer + ISR
├── wifi/
│   ├── CMakeLists.txt
│   ├── wifi.h
│   └── wifi.c          # WiFi station connect + DHCP
└── network/
    ├── CMakeLists.txt
    ├── network.h
    └── network.c       # lwip TCP/UDP client
```

## How it works

1. `uart_init()` configures the BL602 UART peripheral and installs an RX
   interrupt that pushes bytes into a circular buffer.
2. `wifi_init()` brings up the STA interface, connects to the AP in
   `config.h`, and waits for a DHCP lease.
3. `net_init()` opens a TCP (or UDP) socket to `SERVER_IP:SERVER_PORT`.
4. A FreeRTOS task (`forwarding_task`) continuously drains the UART buffer
   and sends the bytes over the network socket. If the link drops it
   automatically reconnects.

## Configure before building

Edit `bl602_uart_wifi_fw/config.h`:

```c
#define WIFI_SSID      "YOUR_SSID"
#define WIFI_PASSWORD  "YOUR_PASSWORD"
#define SERVER_IP      "192.168.1.100"
#define SERVER_PORT    8080
#define USE_TCP        1          /* 1 = TCP, 0 = UDP */
#define UART_BAUD      115200
```

## Build (GitHub Actions)

Push to `main` or run the workflow manually — the
`.github/workflows/build.yml` workflow checks out
[bouffalolab/bouffalo_sdk](https://github.com/bouffalolab/bouffalo_sdk),
configures CMake with the RISC-V toolchain, and produces a flashable
`.bin` artifact.

## Build locally

You need the Bouffalo SDK and the riscv64-unknown-elf toolchain:

```bash
git clone --recursive https://github.com/bouffalolab/bouffalo_sdk.git

cd bl602_uart_wifi_fw
cmake -S . -B build \
  -G Ninja \
  -DBL602_SDK_PATH=$PWD/../bouffalo_sdk \
  -DBL602_CHIP=bl602 \
  -DBL602_BOARD=bl602evb

cmake --build build
```

Outputs: `build/bl602_uart_wifi_fw.bin` (flash) and `.elf` (debug).

## Flash

Use the Bouffalo Lab flash tool (or `blflash`/`dfu-util` depending on your
board's bootloader):

```bash
# Example with the official blflash tool
python3 -m blflash --chip bl602 --port /dev/ttyUSB0 \
  write build/bl602_uart_wifi_fw.bin 0x0
```

## Notes / porting

- The `uart.c`, `wifi.c`, and `network.c` files call bouffalo_sdk APIs
  (`hal_uart_*`, `wifi_mgmr_*`, lwip sockets). Header paths and function
  signatures vary between SDK versions — adjust the `#include` paths to
  match your `bouffalo_sdk` checkout.
- `vTaskDelay`, `pdMS_TO_TICKS`, and `xTaskCreate` assume FreeRTOS is
  running (standard for bouffalo_sdk applications).
- For production use, add framing (start/end delimiters or length prefixes)
  on top of the raw byte stream so the server can segment packets.
