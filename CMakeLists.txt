cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_teclado C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Caso seja a Raspberry Pi Pico W
set(PICO_BOARD pico_w)
#Caso seja a Raspberry Pi Pico 
#set(PICO_BOARD pico)

pico_sdk_init()

add_executable(pico_teclado main.c usb_descriptors.c)
target_include_directories(pico_teclado PRIVATE .)

#Caso seja a Raspberry Pi Pico 
#target_link_libraries(int pico_stdlib)

#Caso seja a Raspberry Pi Pico W
target_link_libraries(pico_teclado pico_cyw43_arch_none pico_stdlib tinyusb_device tinyusb_board)

pico_add_extra_outputs(pico_teclado)