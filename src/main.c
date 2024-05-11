/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lvgl_input_device.h>

#define LOG_LEVEL	CONFIG_LOG_DEFAULT_LEVEL
LOG_MODULE_REGISTER(SmartWatch_OS);

static void lv_btn_click_callback(lv_event_t *e) {
	LOG_INF("Button clicked");
}

int main(void) {

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

    // Log PWM backlight information.
    LOG_INF("PWM device: %s", backlight.dev->name);
    LOG_INF("PWM channel: %d", backlight.channel);
    LOG_INF("PWM period: %d", backlight.period);
    LOG_INF("PWM flags: %d", backlight.flags);

    // Initialize the PWM device.
    int ret = pwm_set_dt(&backlight, 500, 250);
    if (ret < 0) {
        LOG_ERR("Failed to set PWM pulse, exiting...");
        return 0;
    }

    // Initialize the display device with initial GUI.
	if (IS_ENABLED(CONFIG_LV_Z_POINTER_INPUT)) {

        // Create a button.
		lv_obj_t *hello_world_button = lv_btn_create(lv_scr_act());
		lv_obj_align(hello_world_button, LV_ALIGN_CENTER, 0, -15);
		lv_obj_add_event_cb(hello_world_button,
                            lv_btn_click_callback,
                            LV_EVENT_CLICKED, NULL);
		// Create a label for the button.
        lv_obj_t *hello_world_label = lv_label_create(hello_world_button);
        lv_label_set_text(hello_world_label, "Hello world!");
	    lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);
        
        LOG_INF("LVGL initialized.");
	}

    lv_task_handler();
    display_blanking_off(display_dev);

	while (1) {
		lv_task_handler();
		k_sleep(K_MSEC(10));
	}
}