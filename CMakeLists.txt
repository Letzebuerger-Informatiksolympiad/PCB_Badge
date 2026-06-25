# Builds every example in examples/ into its own .uf2 to flash to the badge.
#
# Needs the Raspberry Pi Pico SDK and the arm-none-eabi toolchain installed.
# Point PICO_SDK_PATH at your pico-sdk checkout, then:
#
#   cmake -S . -B build -DPICO_BOARD=pico
#   cmake --build build -j8
#
# The .uf2 files land in build/ (for example build/snake.uf2). Flash one by
# holding BOOT while plugging in the badge and dropping the .uf2 on the drive.

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)

project(weoi_badge_examples C CXX ASM)

pico_sdk_init()

set(ROOT ${CMAKE_CURRENT_LIST_DIR})

file(GLOB EXAMPLE_SRCS ${ROOT}/examples/*.cpp)

foreach(src ${EXAMPLE_SRCS})
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name}
        ${src}
        ${ROOT}/sdk/Badge.cpp
        ${ROOT}/sdk/MainLoop.cpp
        ${ROOT}/sdk/Animation.cpp)
    target_include_directories(${name} PRIVATE ${ROOT} ${ROOT}/examples)
    pico_enable_stdio_uart(${name} 0)
    pico_enable_stdio_usb(${name} 0)
    target_link_libraries(${name} pico_stdlib)
    pico_add_extra_outputs(${name})
endforeach()
