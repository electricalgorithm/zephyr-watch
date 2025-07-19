/** Display initialization implementation for LVGL-based UI setup.
 * Implements display system initialization, theme configuration, and home screen loading for the
 * smartwatch interface.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#include "lvgl.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "userinterface/userinterface.h"
#include "devicetwin/devicetwin.h"

LOG_MODULE_REGISTER(ZephyrWatch_UserInterface, LOG_LEVEL_INF);

// Define the timer callbacks' prototypes.
static void update_clock_view_callback(struct k_timer *timer);

// Define the work queues' prototypes.
static void clock_update_worker(struct k_work *work);
static void date_day_update_worker(struct k_work *work);

static struct k_work_q ui_work_q;
static K_THREAD_STACK_DEFINE(ui_stack_area, 4096);

// Work items for deferred UI tasks
static struct k_work clock_update_work;
static struct k_work date_day_update_work;

// Define timers.
K_TIMER_DEFINE(clock_view_timer, update_clock_view_callback, NULL);

/* USER_INTERFACE_INIT
 * Set-up LVGLs home screen.
 */
void user_interface_init() {
    // Set-up LVGL stuff.
    lv_disp_t *display = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(
        display,
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_RED),
        true,
        LV_FONT_DEFAULT
    );
    lv_disp_set_theme(display, theme);
    home_screen_init();
    lv_disp_load_scr(home_screen);

    // Create a seperate the UI work queue.
    k_work_queue_start(&ui_work_q, ui_stack_area, K_THREAD_STACK_SIZEOF(ui_stack_area),
                       K_PRIO_PREEMPT(5), NULL);
    LOG_DBG("User interface work queue started.");

    // Initialize the work items.
    k_work_init(&clock_update_work, clock_update_worker);
    k_work_init(&date_day_update_work, date_day_update_worker);
    LOG_DBG("The work items are set.");

    // Start timers after work queue is ready - reduced frequency to prevent queue overflow.
    k_timer_start(&clock_view_timer, K_MSEC(2000), K_SECONDS(10));
    LOG_DBG("User interface timers are set.");

    k_work_submit_to_queue(&ui_work_q, &date_day_update_work);
    LOG_DBG("First update signal is send to clock updater.");
}

/* USER_INTERFACE_TASK_HANDLER
 * Call LVGLs task handler.
 */
void user_interface_task_handler() {
    lv_task_handler();
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

/* UPDATE_CLOCK_VIEW_CALLBACK
 * This function is called by the timer to update the clock view.
 * It sends an event to the UI work queue to update the clock view.
 */
static void update_clock_view_callback(struct k_timer *timer) {
    LOG_DBG("Pushing clock_update_work work to ui_work_q.");
    k_work_submit_to_queue(&ui_work_q, &clock_update_work);
}

/* CLOCK_UPDATE_WORKER
 * This function is called by the UI work queue to update the clock view.
 * It updates the current time in the device twin and then
 * calls the home screen set clock function.
 */
static void clock_update_worker(struct k_work *work) {
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
static void date_day_update_worker(struct k_work *work) {
    // Get the device twin to find UTC zone.
    device_twin_t* device_twin = get_device_twin_instance();

    // Construct the local time from UNIX time and save it.
    datetime_t local_time = unix_to_localtime(device_twin->unix_time, device_twin->utc_zone);

    // Update the date and day views using the device twin's current time.
    uint8_t ret = home_screen_set_date(local_time.year, local_time.month, local_time.day);
    if (ret != 0) {
        LOG_ERR("Failed to update the date view.");
    }
    ret = home_screen_set_day(local_time.weekday);
    if (ret != 0) {
        LOG_ERR("Failed to update the day view.");
    }
}