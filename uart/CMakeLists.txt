# UART monitoring module for BL602
set(UART_SRCS
    uart.c
)

add_library(uart_mod STATIC ${UART_SRCS})
target_include_directories(uart_mod PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
)
