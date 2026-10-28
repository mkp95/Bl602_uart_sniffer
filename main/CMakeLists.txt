# Main application sources for BL602 UART-to-WiFi firmware
set(MAIN_SRCS
    main.c
)

add_library(main_app STATIC ${MAIN_SRCS})
target_include_directories(main_app PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
)
