# Main application sources for BL602 UART-to-WiFi firmware
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/main.c)
sdk_add_static_library(main_app)
sdk_library_add_sources(${CMAKE_CURRENT_LIST_DIR}/main.c)
