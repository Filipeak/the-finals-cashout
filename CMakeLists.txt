cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)

project(finals_cashout)

pico_sdk_init()

add_executable(finals_cashout main.c)
target_link_libraries(finals_cashout pico_stdlib hardware_uart)

pico_enable_stdio_usb(finals_cashout 1)
pico_enable_stdio_uart(finals_cashout 0)
pico_add_extra_outputs(finals_cashout)