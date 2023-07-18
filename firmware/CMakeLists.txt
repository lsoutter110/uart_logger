cmake_minimum_required(VERSION 3.13)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(firmware)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(firmware
    main.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(firmware
    pico_stdlib
    pico_multicore
    hardware_uart
    hardware_i2c
    hardware_rtc
    hardware_sync
)

# enable usb output, disable uart output
pico_enable_stdio_usb(firmware 1)
pico_enable_stdio_uart(firmware 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(firmware)
