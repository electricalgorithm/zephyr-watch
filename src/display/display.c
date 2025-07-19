/** Display Subsystem for ZephyrWatch.
 * Provides functions to initialize the display system.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#include "display/display.h"
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

// Get a logger for the display subsystem.
LOG_MODULE_REGISTER(ZephyrWatch_Display, LOG_LEVEL_INF);

// Get devices from the device tree.
#define DISPLAY_DEVICE DT_CHOSEN(zephyr_display)
#define DISPLAY_PWM_DEVICE DT_NODELABEL(pwm_lcd0)

/* ENABLE_DISPLAY_SUBSYSTEM
 * Set the Zephyr display device and set backlight.
 */
int enable_display_subsystem() {
    int ret;

    const struct device *display_dev = DEVICE_DT_GET(DISPLAY_DEVICE);
    ret = device_is_ready(display_dev);
    if (!ret) {
        LOG_ERR("Display device is not ready, exiting... (RET: %d)", ret);
        return 1;
    }
    LOG_DBG("Display device is ready.");

    const struct pwm_dt_spec backlight = PWM_DT_SPEC_GET_BY_IDX(DISPLAY_PWM_DEVICE, 0);
    ret = pwm_is_ready_dt(&backlight);
    if (!ret) {
        LOG_ERR("PWM device is not ready, exiting... (RET: %d)", ret);
        return 1;
    }
    LOG_DBG("PWM device is ready.");

    ret = pwm_set_dt(&backlight, 500, 250);
    if (ret) {
        LOG_ERR("Failed to set PWM pulse, exiting... (RET: %d)", ret);
        return ret;
    }
    LOG_DBG("PWM pulse for LCD backlight set.");

    ret = display_blanking_off(display_dev);
    if (ret) {
        LOG_ERR("Failed to set blanking off, exiting... (RET: %d)", ret);
        return ret;
    }
    LOG_DBG("Set the blanking off.");

    return 0;
}


/* ENABLE_DISPLAY_SUBSYSTEM
 * Disable the Zephyr display device and set backlight to 0.
 */
int disable_display_subsystem() {
    LOG_DBG("Not implemented yet.");
    return 0;
}

/* CHANGE_BRIGHTNESS
 * Change the brightness based on a percentage.
 */
int change_brightness(uint8_t perc) {
    LOG_DBG("Not implemented yet.");
    return 0;
}