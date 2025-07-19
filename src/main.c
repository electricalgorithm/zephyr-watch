/** Main application entry point for Zephyr-based smartwatch firmware.
 * Initializes display, Bluetooth, timers, and manages the core watch functionality including time tracking and UI updates.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
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
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/drivers/counter.h>

#include "display/display.h"
#include "display/screens/home/home.h"
#include "devicetwin/devicetwin.h"
#include "bluetooth/infrastructure.h"
#include "bluetooth/services/current_time_service.h"

#include "datetime/datetime.h"

// Define the logger.
LOG_MODULE_REGISTER(ZephyrWatch, LOG_LEVEL_INF);

// Global values to hold time.
int8_t utc_zone = +2;
const char* weekdays[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

// Define the timer callbacks' prototypes.
void update_clock_view_callback(struct k_timer *timer);
// Define the work queues' prototypes.
void clock_update_worker(struct k_work *work);
void date_day_update_worker(struct k_work *work);

static struct k_work_q ui_work_q;
static K_THREAD_STACK_DEFINE(ui_stack_area, 4096);

// Work items for deferred UI tasks
static struct k_work clock_update_work;
static struct k_work date_day_update_work;

// Define timers.
// K_TIMER_DEFINE(unix_time_timer, update_unix_time_callback, NULL);
K_TIMER_DEFINE(clock_view_timer, update_clock_view_callback, NULL);


int main(void) {
    int ret;

    // Create the device twin.
    device_twin_t* device_twin = create_device_twin_instance(0, utc_zone);
    if (!device_twin) {
        LOG_ERR("Cannot create device twin instance.");
        return 0;
    }
    LOG_DBG("Device twin instance created successfully.");

    // Check if the display device is ready.
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display device is not ready, exiting...");
        return 0;
    }
    LOG_DBG("Display device is ready.");

    const struct pwm_dt_spec backlight = PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(pwm_lcd0), 0);
    if (!pwm_is_ready_dt(&backlight)) {
        LOG_ERR("PWM device is not ready, exiting...");
        return 0;
    }
    LOG_DBG("PWM device is ready.");

    // Initialize the PWM device.
    ret = pwm_set_dt(&backlight, 500, 250);
    if (ret < 0) {
        LOG_ERR("Failed to set PWM pulse, exiting...");
        return 0;
    }
    LOG_DBG("PWM pulse for LCD backlight set.");

    // Initialize the display device with initial GUI.
    display_init();
    LOG_DBG("UI initialized.");

    // Start the UI work queue.
    k_work_queue_start(&ui_work_q, ui_stack_area, K_THREAD_STACK_SIZEOF(ui_stack_area),
    K_PRIO_PREEMPT(5), NULL);
    LOG_DBG("UI work queue started.");

    // Initialize the work items.
    k_work_init(&clock_update_work, clock_update_worker);
    k_work_init(&date_day_update_work, date_day_update_worker);

    // Start timers after work queue is ready - reduced frequency to prevent queue overflow.
    k_timer_start(&clock_view_timer, K_MSEC(2000), K_SECONDS(10));
    k_work_submit_to_queue(&ui_work_q, &date_day_update_work);
    LOG_DBG("Timers are set.");

    // Initialize LVGL first and let UI stabilize
    lv_task_handler();
    display_blanking_off(display_dev);

    ret = enable_datetime_subsystem();
    if (ret) {
        LOG_ERR("Datetime subsystem couldn't enabled. (RET: %d)", ret);
        return ret;
    }
    LOG_DBG("Datetime subsystem is enabled.");

    // Initialize the Bluetooth stack.
    // Give the system more time to stabilize before initializing Bluetooth.
    k_sleep(K_SECONDS(2));
    ret = enable_bluetooth_and_start_advertisement();
    if (ret) {
        LOG_ERR("Bluetooth subsystem couldn't enabled. (RET: %d)", ret);
        return ret;
    }
    LOG_DBG("Bluetooth subsystem is enabled.");

    while (1) {
        lv_task_handler();
        k_sleep(K_MSEC(20)); // Increased sleep time to reduce system load
    }
}

/* UPDATE_CLOCK_VIEW_CALLBACK
 * This function is called by the timer to update the clock view.
 * It sends an event to the UI work queue to update the clock view.
 */
void update_clock_view_callback(struct k_timer *timer) {
    LOG_DBG("Pushing clock_update_work work to ui_work_q.");
    k_work_submit_to_queue(&ui_work_q, &clock_update_work);
}

/* TRIGGER_UI_CHANGE
 * Function to update the UI from external sources (like Bluetooth CTS).
 * This function will be called by the external sources.
 */
void trigger_ui_update() {
    // Only submit if not already pending.
    if (!k_work_is_pending(&clock_update_work)) {
        k_work_submit_to_queue(&ui_work_q, &clock_update_work);
    }
    if (!k_work_is_pending(&date_day_update_work)) {
        k_work_submit_to_queue(&ui_work_q, &date_day_update_work);
    }
}


/* CLOCK_UPDATE_WORKER
 * This function is called by the UI work queue to update the clock view.
 * It updates the current time in the device twin and then
 * calls the home screen set clock function.
 */
void clock_update_worker(struct k_work *work) {
    // Get the device twin to use the latest information..
    device_twin_t* device_twin = get_device_twin_instance();
    LOG_DBG("Device's clock in UNIX epochs: %u", device_twin->unix_time);

    // Construct the local time from UNIX time and save it.
    datetime_t local_time = unix_to_localtime(device_twin->unix_time, device_twin->utc_zone);

    // Update the clock view using the device twin's current time.
    uint8_t ret = home_screen_set_clock(local_time.hour, local_time.minute);
    if (ret != 0) {
        LOG_ERR("Failed to update the clock view.");
    }

    // Send a date day update worker if 00:00.
    if (local_time.hour == 0 && local_time.minute == 0) {
        k_work_submit_to_queue(&ui_work_q, &date_day_update_work);
        LOG_DBG("Date and day update worker submitted to queue.");
    }
}

/* DATE_DAY_UPDATE_WORKER
 * This function is called by the UI work queue to update the date and day view.
 * It updates the current time in the device twin and then
 * calls the home screen set date function.
 */
void date_day_update_worker(struct k_work *work) {
    // Get the device twin to find UTC zone.
    device_twin_t* device_twin = get_device_twin_instance();

    // Construct the local time from UNIX time and save it.
    datetime_t local_time = unix_to_localtime(device_twin->unix_time, device_twin->utc_zone);

    // Update the date and day views using the device twin's current time.
    uint8_t ret = home_screen_set_date(local_time.year, local_time.month, local_time.day);
    if (ret != 0) {
        LOG_ERR("Failed to update the date view.");
    }
    ret = home_screen_set_day(weekdays[local_time.weekday]);
    if (ret != 0) {
        LOG_ERR("Failed to update the day view.");
    }
}