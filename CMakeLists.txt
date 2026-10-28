cmake_minimum_required(VERSION 3.16)

project(bl602_uart_wifi_fw C ASM)

# ---------------------------------------------------------------------------
# BL602 UART-to-WiFi firmware
# ---------------------------------------------------------------------------
# Build with the Bouffalo Lab bouffalo_sdk.
#
#   BL602_SDK_PATH  : path to bouffalo_sdk (required)
#   BL602_CHIP      : bl602 (default)
#   BL602_BOARD     : bl602evb (default)
#
# Example (inside the SDK build tree):
#   cmake -DBL602_SDK_PATH=../bouffalo_sdk -DBL602_CHIP=bl602 \
#         -DBL602_BOARD=bl602evb -G "Unix Makefiles" ..
#   make
# ---------------------------------------------------------------------------

if(NOT DEFINED BL602_SDK_PATH)
    message(FATAL_ERROR "BL602_SDK_PATH not set. Point it at your bouffalo_sdk checkout.")
endif()

set(BL602_CHIP "bl602" CACHE STRING "BL602 chip name")
set(BL602_BOARD "bl602evb" CACHE STRING "BL602 board name")

# Toolchain file provided by the SDK (if it exists); otherwise CMake will
# use the RISC-V compiler found on PATH (e.g. xpack riscv-none-elf-gcc).
if(EXISTS "${BL602_SDK_PATH}/cmake/toolchain/riscv-gcc.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${BL602_SDK_PATH}/cmake/toolchain/riscv-gcc.cmake"
        CACHE FILEPATH "RISC-V GCC toolchain for BL602")
endif()

# Include SDK build rules
include("${BL602_SDK_PATH}/cmake/bouffalo_sdk.cmake")

# ---------------------------------------------------------------- components
add_subdirectory(main)
add_subdirectory(uart)
add_subdirectory(wifi)
add_subdirectory(network)

# ---------------------------------------------------------------- firmware
bl602_add_firmware(
    NAME bl602_uart_wifi_fw
    CHIP ${BL602_CHIP}
    BOARD ${BL602_BOARD}
    LIBS main_app uart_mod wifi_mod net_mod
)

# Output artifacts: .bin (flashable) and .elf (debug)
bl602_generate_bin(bl602_uart_wifi_fw)
