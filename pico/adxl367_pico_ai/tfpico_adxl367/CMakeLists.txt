# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.12)
include(vendor/pico_sdk_import.cmake)
project(tfpico-adxl367 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
pico_sdk_init()

add_executable(tfpico_adxl367 main.cpp spi_adxl367.cpp)
target_link_libraries(tfpico_adxl367 PRIVATE
  hardware_spi
  hardware_sync
  pico_stdlib
  pico-tflmicro
)
pico_enable_stdio_usb(tfpico_adxl367 1)
pico_add_extra_outputs(tfpico_adxl367)

# include target library pico-tflmicro
add_subdirectory(vendor/pico-tflmicro EXCLUDE_FROM_ALL)
