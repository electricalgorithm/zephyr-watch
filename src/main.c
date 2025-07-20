/** Main application entry point for Zephyr-based smartwatch firmware.
 * Initializes display, Bluetooth, timers, and manages the core watch functionality including time tracking and UI updates.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "watchdog/watchdog.h"
#include "display/display.h"
#include "devicetwin/devicetwin.h"
#include "userinterface/userinterface.h"
#include "datetime/datetime.h"
#include "bluetooth/infrastructure.h"

// Define the logger.
LOG_MODULE_REGISTER(ZephyrWatch, LOG_LEVEL_INF);

#define SLEEP_UI_STABILIZE_MS 2000
#define SLEEP_MAIN_CORE_MS 20

// Setting for device's time zone.
int8_t utc_zone = +2;

int main(void) {
    int ret;

    // Set-up watchdog before all the subsystems.
    ret = enable_watchdog_subsystem();
    if (ret) {
        LOG_ERR("Watchdog subsystem couldn't be enabled. (RET: %d)", ret);
        return ret;
    }
    LOG_INF("Watchdog system is enabled.");

    // Create the device twin.
    device_twin_t* device_twin = create_device_twin_instance(0, utc_zone);
    if (!device_twin) {
        LOG_ERR("Cannot create device twin instance.");
        return 0;
    }
    LOG_INF("Device twin instance created successfully.");

    // Init the display subsystem.
    ret = enable_display_subsystem();
    if (ret) {
        LOG_ERR("Display subsystem couldn't enabled. (RET: %d)", ret);
        return ret;
    }
    LOG_INF("Display subsystem is enabled.");

    // Initialize the display device with initial user interface.
    user_interface_init();
    LOG_INF("User interface subsystem is enabled.");

    // Refresh the UI.
    user_interface_task_handler();
    LOG_INF("User interface is refreshed initally.");

    // Enable datetime subsystem.
    ret = enable_datetime_subsystem();
    if (ret) {
        LOG_ERR("Datetime subsystem couldn't enabled. (RET: %d)", ret);
        return ret;
    }
    LOG_INF("Datetime subsystem is enabled.");

    // Initialize the Bluetooth stack.
    // Give the system more time to stabilize before initializing Bluetooth.
    k_sleep(K_MSEC(SLEEP_UI_STABILIZE_MS));
    ret = enable_bluetooth_subsystem();
    if (ret) {
        LOG_ERR("Bluetooth subsystem couldn't enabled. (RET: %d)", ret);
        return ret;
    }
    LOG_INF("Bluetooth subsystem is enabled.");

    while (1) {
        user_interface_task_handler();
        k_sleep(K_MSEC(SLEEP_MAIN_CORE_MS));

        // Kick the watchdog.
        kick_watchdog();
    }
}