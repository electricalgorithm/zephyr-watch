<p align="center">
  <h1 align="center">ZephyrWatch</h1>
</p>
<p align="center">
an open-source smartwatch based on lvgl, zephyr and ble
</p>

<div align="center">

![version](https://img.shields.io/github/v/release/electricalgorithm/zephyr-watch?include_prereleases&sort=date&display_name=release&style=flat-square) ![](https://img.shields.io/badge/license-GNUv3-critical?style=flat-square) ![stars](https://img.shields.io/github/stars/electricalgorithm/zephyr-watch?style=flat-square) ![GitHub milestone details](https://img.shields.io/github/milestones/progress-percent/electricalgorithm/zephyr-watch/2?style=flat-square&color=%23ffcc01)



---

</div>

### Features
- Real-Time Counter to Track the Time
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
