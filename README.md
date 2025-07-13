# ESP32 Zephyr RTOS Watch

**Supported Boards:**
- [ESP32-S3-Touch-LCD-1.28](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.28)

## Build Instructions
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