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
#include "display/screens/home/home.h"
#include "timeutils/timeutils.h"
#include "devicetwin/devicetwin.h"

// Define the logger.
LOG_MODULE_REGISTER(ZephyrWatch, LOG_LEVEL_DBG);

// Global value to hold time value in UNIX time.
uint32_t unix_time = 1748554674;
int8_t utc_zone = +1;

// Define timers to track real time.
void update_unix_time_callback(struct k_timer *timer);
void update_clock_view_callback(struct k_timer *timer);
void update_date_day_view_callback(struct k_timer *timer);
K_TIMER_DEFINE(unix_time_timer, update_unix_time_callback, NULL);
K_TIMER_DEFINE(clock_view_timer, update_clock_view_callback, NULL);
K_TIMER_DEFINE(date_day_view_timer, update_date_day_view_callback, NULL);

// Define the weekday names.
const char* weekdays[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

int main(void) {
	int ret;

    // Create the device twin.
    device_twin_t* device_twin = create_device_twin_instance(unix_to_localtime(unix_time, utc_zone), utc_zone);
    if (!device_twin) {
        LOG_ERR("Cannot create device twin instance.");
        return 0;
    }

	// Start the timer to track the real time.
	k_timer_start(&unix_time_timer, K_MSEC(60), K_MSEC(1000));
	k_timer_start(&clock_view_timer, K_MSEC(60), K_SECONDS(30));
    k_timer_start(&date_day_view_timer, K_MSEC(60), K_MINUTES(1));

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
    // Get the device twin to find UTC zone.
    device_twin_t* device_twin = get_device_twin_instance();

    // Construct the local time from UNIX time and save it.
    utc_time_t local_time = unix_to_localtime(unix_time, device_twin->utc_zone);
    device_twin->current_time = local_time;

    // Log the current time.
    LOG_INF("Setting new time. %04u-%02u-%02u %02u:%02u:%02u (%s) [UTC %d]",
        device_twin->current_time.year,
        device_twin->current_time.month,
        device_twin->current_time.day,
        device_twin->current_time.hour,
        device_twin->current_time.minute,
        device_twin->current_time.second,
        weekdays[device_twin->current_time.weekday],
        device_twin->utc_zone
    );

    // Update the clock view.
    uint8_t ret = home_screen_set_clock(device_twin->current_time.hour, device_twin->current_time.minute);
    if (ret != 0) {
        LOG_ERR("Failed to update the clock view.");
    }
}

/* UPDATE_DATE_DAY_VIEW_CALLBACK
 */
void update_date_day_view_callback(struct k_timer *timer) {
    // Get the device twin to find UTC zone.
    device_twin_t* device_twin = get_device_twin_instance();

    // Construct the local time from UNIX time and save it.
    utc_time_t local_time = unix_to_localtime(unix_time, device_twin->utc_zone);
    device_twin->current_time = local_time;

    // Update the date view.
    uint8_t ret = home_screen_set_date(
        device_twin->current_time.year,
        device_twin->current_time.month,
        device_twin->current_time.day
    );
    if (ret != 0) {
        LOG_ERR("Failed to update the date view.");
    }

    // Update the day view.
    ret = home_screen_set_day(weekdays[device_twin->current_time.weekday]);
    if (ret != 0) {
        LOG_ERR("Failed to update the day view.");
    }
}