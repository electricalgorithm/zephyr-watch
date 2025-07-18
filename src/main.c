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
#include "timeutils/timeutils.h"
#include "devicetwin/devicetwin.h"
#include "bluetooth/infrastructure.h"
#include "bluetooth/services/current_time_service.h"

// Define the logger.
LOG_MODULE_REGISTER(ZephyrWatch, LOG_LEVEL_INF);

// Global values to hold time.
uint32_t unix_time = 1748554674;
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

// Create an alarm for 1 second.
#define ALARM_INTERVAL_US 1000000
#define ALARM_CHANNEL_ID 0


void rtc_counter_isr(
    const struct device *dev,
    uint8_t channel_id, 
    uint32_t ticks,
    void *user_data)
{
    // Cast alarm config from user data.
    struct counter_alarm_cfg *alarm_cfg = user_data;

    // Reset alarm
    alarm_cfg->ticks = counter_us_to_ticks(dev, ALARM_INTERVAL_US);
    counter_set_channel_alarm(dev, ALARM_CHANNEL_ID, alarm_cfg);
    
    // Update the second.
    unix_time++;
}


int main(void) {
	int ret;

    // Create the device twin.
    device_twin_t* device_twin = create_device_twin_instance(unix_to_localtime(unix_time, utc_zone), utc_zone);
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

    // Give the system more time to stabilize before initializing Bluetooth.
    k_sleep(K_SECONDS(2));

    // Initialize the Bluetooth stack.
    ret = enable_bluetooth_and_start_advertisement();
    if (ret) {
        LOG_ERR("Bluetooth init failed (ret %d).", ret);
        return ret;
    }
    LOG_INF("Started to advertise with Bluetooth.");

    // Set the real time counter to trigger an callback every second.
    const struct device *real_time_counter = DEVICE_DT_GET(DT_NODELABEL(rtc_timer));
    if (!device_is_ready(real_time_counter)) {
        LOG_ERR("Real time counter device is not ready.");
        return -ENODEV;
    }
    LOG_DBG("Real time counter device is ready.");

    // Configure real time counter to track tine.
    ret = counter_start(real_time_counter);
    if (ret) {
        LOG_ERR("Failed to start real time counter (ret %d).", ret);
        return ret;
    }
    LOG_DBG("Real time counter started successfully.");

    struct counter_alarm_cfg alarm_cfg = {
        .flags = 0,
        .ticks = counter_us_to_ticks(real_time_counter, ALARM_INTERVAL_US),
        .callback = rtc_counter_isr,
        .user_data = &alarm_cfg,
    };
    ret = counter_set_channel_alarm(real_time_counter, ALARM_CHANNEL_ID, &alarm_cfg);
    if (ret) {
        LOG_ERR("Failed to set channel alarm (ret %d).", ret);
        return ret;
    }
    LOG_DBG("Channel alarm set successfully.");

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

/* UPDATE_GLOBAL_UNIX_TIME
 * Function to update the global unix time from external sources
 * (like Bluetooth CTS). This function will be called by the external
 * sources.
 */
void update_global_unix_time(uint32_t unix_timestamp) {
    unix_time = unix_timestamp;
    LOG_INF("unix_time is updated to: %u", unix_time);

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
    LOG_DBG("unix_time=%u", unix_time);
    // Get the device twin to find UTC zone.
    device_twin_t* device_twin = get_device_twin_instance();

    // Construct the local time from UNIX time and save it.
    utc_time_t local_time = unix_to_localtime(unix_time, device_twin->utc_zone);
    device_twin->current_time = local_time;

    // Update the clock view using the device twin's current time.
    uint8_t ret = home_screen_set_clock(device_twin->current_time.hour, device_twin->current_time.minute);
    if (ret != 0) {
        LOG_ERR("Failed to update the clock view.");
    }

    // Send a date day update worker if 00:00.
    if (device_twin->current_time.hour == 0 && device_twin->current_time.minute == 0) {
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
    utc_time_t local_time = unix_to_localtime(unix_time, device_twin->utc_zone);
    device_twin->current_time = local_time;

    // Update the date and day views using the device twin's current time.
    uint8_t ret = home_screen_set_date(device_twin->current_time.year, device_twin->current_time.month, device_twin->current_time.day);
    if (ret != 0) {
        LOG_ERR("Failed to update the date view.");
    }
    ret = home_screen_set_day(weekdays[device_twin->current_time.weekday]);
    if (ret != 0) {
        LOG_ERR("Failed to update the day view.");
    }
}