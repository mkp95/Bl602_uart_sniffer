cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------------------
# BL602 UART-to-WiFi firmware
# ---------------------------------------------------------------------------
# Build with the Bouffalo Lab bouffalo_sdk (follows the examples/ pattern).
#
#   BL602_SDK_PATH  : path to bouffalo_sdk (required)
#   CROSS_COMPILE   : riscv toolchain prefix (e.g. riscv-none-elf-)
#
# Example:
#   export BL_SDK_BASE=$PWD/bouffalo_sdk
#   export CROSS_COMPILE=riscv-none-elf-
#   cmake -S bl602_uart_wifi_fw -B build
#   cmake --build build
# ---------------------------------------------------------------------------

if(NOT DEFINED BL602_SDK_PATH)
    message(FATAL_ERROR "BL602_SDK_PATH not set. Point it at your bouffalo_sdk checkout.")
endif()

set(ENV{BL_SDK_BASE} "${BL602_SDK_PATH}")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# ---------------------------------------------------------------- components
add_subdirectory(main)
add_subdirectory(uart)
add_subdirectory(wifi)
add_subdirectory(network)

# ---------------------------------------------------------------- firmware
# The SDK's project() macro (from extension.cmake) builds the ELF + BIN
project(bl602_uart_wifi_fw C ASM)
