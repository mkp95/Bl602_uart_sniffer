cmake_minimum_required(VERSION 3.16)
project(bl602_uart_wifi_fw C ASM)

# ---------------------------------------------------------------------------
# BL602 UART-to-WiFi firmware — standalone build
# ---------------------------------------------------------------------------
# Builds our source files directly with the RISC-V toolchain.
#
#   BL602_SDK_PATH  : path to bouffalo_sdk (for headers, optional)
#   CROSS_COMPILE   : riscv toolchain prefix (e.g. riscv-none-elf-)
#
# Example:
#   export CROSS_COMPILE=riscv-none-elf-
#   cmake -S . -B build -G Ninja
#   cmake --build build
# ---------------------------------------------------------------------------

# Toolchain prefix
if(NOT DEFINED CROSS_COMPILE)
    set(CROSS_COMPILE "riscv-none-elf-")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER   ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_LINKER       ${CROSS_COMPILE}ld)
set(CMAKE_AR           ${CROSS_COMPILE}ar)
set(CMAKE_OBJCOPY      ${CROSS_COMPILE}objcopy)
set(CMAKE_OBJDUMP      ${CROSS_COMPILE}objdump)
set(CMAKE_SIZE         ${CROSS_COMPILE}size)
set(CMAKE_RANLIB       ${CROSS_COMPILE}ranlib)

# Cross-compile settings
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Compiler flags for BL602 (RISC-V, no standard library linking, but keep
# standard headers for stdint.h/stddef.h etc.)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv32imac -mabi=ilp32 -Wall -Wextra -Os -ffunction-sections -fdata-sections -fno-builtin-printf -nostdlib -nodefaultlibs")

# ---------------------------------------------------------------- includes
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/main
    ${CMAKE_CURRENT_LIST_DIR}/uart
    ${CMAKE_CURRENT_LIST_DIR}/wifi
    ${CMAKE_CURRENT_LIST_DIR}/network
)

# SDK headers (if available)
if(DEFINED BL602_SDK_PATH AND EXISTS "${BL602_SDK_PATH}")
    include_directories(${BL602_SDK_PATH})
endif()

# ---------------------------------------------------------------- sources
set(FIRMWARE_SRCS
    startup.S
    main/main.c
    uart/uart.c
    wifi/wifi.c
    network/network.c
)

add_executable(bl602_uart_wifi_fw.elf ${FIRMWARE_SRCS})

# Linker script for BL602 memory layout
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/bl602.ld")
    target_link_options(bl602_uart_wifi_fw.elf PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/bl602.ld)
else()
    message(WARNING "bl602.ld not found — using default linker script")
endif()

target_link_options(bl602_uart_wifi_fw.elf PRIVATE
    -nostartfiles -Wl,--gc-sections -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/bl602_uart_wifi_fw.map
)

# ---------------------------------------------------------------- binary output
add_custom_command(TARGET bl602_uart_wifi_fw.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:bl602_uart_wifi_fw.elf>
            ${CMAKE_CURRENT_BINARY_DIR}/bl602_uart_wifi_fw.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:bl602_uart_wifi_fw.elf>
    COMMENT "Generating bl602_uart_wifi_fw.bin"
)
