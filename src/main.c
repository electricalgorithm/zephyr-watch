/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#include "display/display.h"
#include "timeutils/timeutils.h"

// Define the logger.
LOG_MODULE_REGISTER(ZephyrWatch, LOG_LEVEL_DBG);

// Global value to hold time value in UNIX time.
uint32_t unix_time = 1748554674;

// Define timers to track real time.
void update_unix_time_callback(struct k_timer *timer);
void update_clock_view_callback(struct k_timer *timer);
K_TIMER_DEFINE(unix_time_timer, update_unix_time_callback, NULL);
K_TIMER_DEFINE(clock_view_timer, update_clock_view_callback, NULL);

// Define the weekday names.
const char* weekdays[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

int main(void) {
	int ret;

	// Start the timer to track the real time.
	k_timer_start(&unix_time_timer, K_MSEC(0), K_MSEC(1000));
	k_timer_start(&clock_view_timer, K_SECONDS(60), K_SECONDS(60));

    // Check if the display device is ready.
	const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display device is not ready, exiting...");
		return 0;
	}
	LOG_INF("Display device is ready.");

    const struct pwm_dt_spec backlight = PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(pwm_lcd0), 0);
    if (!pwm_is_ready_dt(&backlight)) {
        LOG_ERR("PWM device is not ready, exiting...");
        return 0;
    }
	LOG_INF("PWM device is ready.");

    // Initialize the PWM device.
    ret = pwm_set_dt(&backlight, 500, 250);
    if (ret < 0) {
        LOG_ERR("Failed to set PWM pulse, exiting...");
        return 0;
    }
	LOG_INF("PWM pulse for LCD backlight set.");

    // Initialize the display device with initial GUI.
    display_init();
	LOG_INF("UI initialized.");

    lv_task_handler();
    display_blanking_off(display_dev);

	while (1) {
		lv_task_handler();
        k_sleep(K_SECONDS(1));
	}
}

/* UPDATE_UNIX_TIME_CALLBACK
 * This timer callback function runs every second to update
 * the value inside the unix_time to track the real time.
 * It is neccessary to use timers instead of configuring the
 * RTC because the PoC board does not have RTC support in Zephyr.
 */
void update_unix_time_callback(struct k_timer *timer) {
	unix_time += 1;
}

/* UPDATE_CLOCK_VIEW_CALLBACK
 * This timer callback function runs every minute to update
 * the clock view in the LVGL UI by using the unix_time variable.
 */
void update_clock_view_callback(struct k_timer *timer) {
    utc_time_t local = unix_to_localtime(unix_time, +1); // UTC+1
	LOG_INF("Local : %04u-%02u-%02u %02u:%02u:%02u (%s) [UTC+1]",
           local.year, local.month, local.day, local.hour, local.minute, local.second,
           weekdays[local.weekday]);
}