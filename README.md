# ESP32 Zephyr RTOS Watch

This is a smart-watch project based on ESP32, LVGL and Zephyr RTOS. I started it as a side project to learn about Zephyr, BLE and LVGL. As you can imagine, it was a big kick to learn about many stuff at oonce. Now, I'm proud to present you. Feel free to contribute and/or contact to me!

### Features
- LVGL for UI and Graphics Rendering
- BLE Current Time Service (GATT) for Time Synchronization
- BLE Device Information Service (DIS) for Device Metadata

### Supported Boards
- [ESP32-S3-Touch-LCD-1.28](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.28)

## Tools
We created another repository called [ZephyrWatchBLETools](https://github.com/electricalgorithm/ZephyrWatchBLETools) to publish all the utilities needed to use ZephyrWatch. Currently, the list is very limited, however, I plan to expand it in the future.

- updatetime.py: A script to synchronize the time on the ZephyrWatch with the computer's time.


## Build
1. Install the Zephyr according the information in their website.
2. Get the ESP32 HAL using `west blobs fetch hal_espressif`.
3. Clone the package and build it using the instructions provided.
```sh
$ west build -p always . --board esp32s3_touch_lcd_1_28/esp32s3/procpu
```
4. After the build, flash the firmware to the board.
```sh
$ west flash
```
5. All done!

## Monitoring
```sh
$ west espressif monitor
```