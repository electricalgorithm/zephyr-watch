set(BOARD esp32s3_touch_lcd_1_28/esp32s3/procpu)
set(DTC_OVERLAY_FILE boards/esp32.overlay)

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Esp32SmartWatch)

file(GLOB_RECURSE app_sources src/*.c)

target_sources(app PRIVATE ${app_sources})
target_include_directories(app PRIVATE src/)