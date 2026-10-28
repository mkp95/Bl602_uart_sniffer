# Network client module (TCP/UDP) for BL602
sdk_add_static_library(net_mod)
sdk_library_add_sources(${CMAKE_CURRENT_LIST_DIR}/network.c)
