/** Watchdog Subsystem for ZephyrWatch.
 * Provides functions to initialize the watchdog and kick it.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/watchdog.h>

#include "watchdog/watchdog.h"

// Create a logger.
LOG_MODULE_REGISTER(ZephyrWatch_Watchdog, LOG_LEVEL_INF);

// Get the watchdog device using the project's aliases.
#define WATCHDOG_DEVICE DT_ALIAS(watchdogdevice)
#define WATCHDOG_TIMEOUT_MS 30000

// Store the variables for kicking it.
static const struct device *watchdog_device = DEVICE_DT_GET(WATCHDOG_DEVICE);
static int channel_id;

/* ENABLE_WATCHDOG_SUBSYSTEM
 * Prepare the watchdog device in the system. Call it before all the subsystems.
 */
int enable_watchdog_subsystem() {
    int ret;

    // Check the watchdog device if its ready.
    if (!device_is_ready(watchdog_device)) {
        LOG_ERR("Watchdog Timer device is not ready, exiting,");
        return 1;
    }
    LOG_DBG("Watchdog Timer device is ready.");

    // Create the watchdog config struct to pass to timeouts. 
    struct wdt_timeout_cfg watchdog_config = {
        .window = {
            .min = 0,                       // Watchdog can be kicked anytime.
            .max = WATCHDOG_TIMEOUT_MS,     // The max amount of wait before a reboot.
        },
        .callback = NULL,                   // No need to run a callback beforehand.
        .flags = WDT_FLAG_RESET_SOC,        // Reset all the CPUs *whole SOC*.
    };

    // Install the first watchdog timer.
    ret = wdt_install_timeout(watchdog_device, &watchdog_config);
    if (ret < 0) {
        LOG_ERR("Watchdog timeout registry couldn't be configured. (RET: %d)", ret);
        return ret;
    }

    // Save the channel id since it'll be needed when kicking it.
    channel_id = ret;
    LOG_DBG("Watchdog timeout registry is completed. Channel ID: %d", channel_id);
    
    // Set-up the timers.
    ret = wdt_setup(watchdog_device, WDT_OPT_PAUSE_HALTED_BY_DBG);
    if (ret) {
        LOG_ERR("Watchdog set-up couldn't completed. (RET: %d)", ret);
        return ret;
    }
    LOG_DBG("Watchdog set-up is completed.");

    return ret;
}

/* DISABLE_WATCHDOG_SUBSYSTEM
 * Deregister all watchdog timeouts and remove the subsystem.
 */
int disable_watchdog_subsystem() {
    int ret = wdt_disable(watchdog_device);
    if (ret) LOG_ERR("Could not disable watchdog timers. (RET: %d)", ret);
    return ret;
}

/* KICK_WATCHDOG
 * Kick ("send signal") to watchdog timer to indicate responsiveness.
 */
void kick_watchdog() {
    int ret = wdt_feed(watchdog_device, channel_id);
    if (ret) {
        LOG_ERR("Couldn't kick watchdog timer. (RET: %d)", ret);
    }
}