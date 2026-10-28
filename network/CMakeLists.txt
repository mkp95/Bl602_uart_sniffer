# Network client module (TCP/UDP) for BL602
set(NET_SRCS
    network.c
)

add_library(net_mod STATIC ${NET_SRCS})
target_include_directories(net_mod PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
)
