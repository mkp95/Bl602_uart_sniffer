cmake_minimum_required(VERSION 3.16)

# ---------------------------------------------------------------------------
# BL602 UART-to-WiFi firmware
# ---------------------------------------------------------------------------
# Build with the Bouffalo Lab bouffalo_sdk.
#
#   BL602_SDK_PATH  : path to bouffalo_sdk (required)
#   BL602_CHIP      : bl602 (default)
#   BL602_BOARD     : bl602evb (default)
#   CROSS_COMPILE   : riscv toolchain prefix (e.g. riscv-none-elf-)
#
# Example:
#   export BL_SDK_BASE=$PWD/bouffalo_sdk
#   export CROSS_COMPILE=riscv-none-elf-
#   cmake -S bl602_uart_wifi_fw -B build -G Ninja
#   cmake --build build
# ---------------------------------------------------------------------------

if(NOT DEFINED BL602_SDK_PATH)
    message(FATAL_ERROR "BL602_SDK_PATH not set. Point it at your bouffalo_sdk checkout.")
endif()

set(BL602_CHIP "bl602" CACHE STRING "BL602 chip name")
set(BL602_BOARD "bl602evb" CACHE STRING "BL602 board name")

# Export BL_SDK_BASE for the SDK config (it reads this env var)
set(ENV{BL_SDK_BASE} "${BL602_SDK_PATH}")

# Include the SDK config which sets up toolchain, extension macros, and builds SDK
include("${BL602_SDK_PATH}/cmake/bouffalo_sdk-config.cmake")

# ---------------------------------------------------------------- components
# Register each component as a static library using sdk_add_static_library

add_subdirectory(main)
add_subdirectory(uart)
add_subdirectory(wifi)
add_subdirectory(network)

# ---------------------------------------------------------------- firmware
# The SDK's project() macro (overridden in extension.cmake) creates the final
# ELF + BIN. It expects sdk_set_main_file() to have been called and libraries
# registered via sdk_add_static_library().

project(bl602_uart_wifi_fw C ASM)
