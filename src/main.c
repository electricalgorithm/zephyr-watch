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
#include <zephyr/drivers/rtc.h>

#include "user_interface/ui.h"
#include "bluetooth/current_time_service.h"

LOG_MODULE_REGISTER(ZephyrWatch, LOG_LEVEL_INF);


int main(void) {
	int ret;

	// Initialize the RTC device.
	const struct device *rtc_dev = DEVICE_DT_GET(DT_NODELABEL(rtc));
	if (!device_is_ready(rtc_dev)) {
		LOG_ERR("RTC device is not ready, exiting...");
		return 0;
	}
	LOG_INF("RTC device is ready.");

	// struct rtc_time time = {
	// 	.tm_sec = 0,
	// 	.tm_min = 58,
	// 	.tm_hour = 0,
	// 	.tm_mday = 15,
	// 	.tm_mon = 5,
	// 	.tm_year = 2024,
	// 	.tm_wday = 4,
	// 	.tm_yday = 0,
	// 	.tm_isdst = 0,
	// 	.tm_nsec = 0,
	// };

	// ret = rtc_set_time(rtc_dev, &time);
	// if (ret < 0) {
	// 	LOG_ERR("Failed to set time to RTC, exiting...");
	// 	return 0;
	// }
	// LOG_INF("Time set to RTC.");

	// ret = rtc_get_time(rtc_dev, &time);
	// if (ret < 0) {
	// 	LOG_ERR("Failed to get time from RTC, exiting...");
	// 	return 0;
	// }
	//LOG_INF("Current time: %d-%d-%d %d:%d:%d", time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

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
    ui_init();
	LOG_INF("UI initialized.");

    lv_task_handler();
    display_blanking_off(display_dev);

	while (1) {
		lv_task_handler();
        k_sleep(K_USEC(10));
	}
}
