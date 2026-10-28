# WiFi connection module for BL602
set(WIFI_SRCS
    wifi.c
)

add_library(wifi_mod STATIC ${WIFI_SRCS})
target_include_directories(wifi_mod PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
)
