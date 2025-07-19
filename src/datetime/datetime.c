/** Datetime Subsystem for ZephyrWatch
 * It is a real-time clock implementation based on real-time counters for ESP32.
 * 
 * @license: GNU v3
 * @maintainer: electricalgorithm @ github
*/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>

#include "devicetwin/devicetwin.h"
#include "datetime/datetime.h"

// Get devices from the device tree.
#define RTC_COUNTER_DEVICE DT_ALIAS(rtccounterdevice)

// Configuration for counter alarm.
#define ALARM_INTERVAL_US 1000000
#define ALARM_CHANNEL_ID 0

/* Register a logger for this library. */
LOG_MODULE_REGISTER(ZephyrWatch_Datetime, LOG_LEVEL_INF);

/* Disable flag to not set alarm again in ISR.
 * 0: Set alarm again.
 * !: Do not set alarm again.
 */
static uint8_t reset_alarm = 0;

/* Global alarm configuration structure - must persist for ISR access */
static struct counter_alarm_cfg alarm_cfg;

/* Prototype definition of internal static functions and variables */
static const uint16_t days_in_month[] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
};
static bool is_leap_year(uint16_t year);
static uint8_t calc_weekday(uint32_t days_since_epoch);
// static datetime_t unix_to_localtime(int32_t timestamp, int8_t utc_offset_hours);
// static datetime_t unix_to_utc(uint32_t timestamp);

/* RTC_ISR
 * Interrupt service routine for alarm with real-time counters.  This ISR is executed every
 * seconds. The user_data is an counter_alarm_cfg object. It updates static unix_time variable.
 */
void rtc_isr(const struct device *dev, uint8_t channel_id, uint32_t ticks, void *user_data) {
    // Cast alarm config from user data.
    struct counter_alarm_cfg *alarm_cfg = user_data;

    // Reset alarm if flag is set.
    if (!reset_alarm) {
        alarm_cfg->ticks = counter_us_to_ticks(dev, ALARM_INTERVAL_US);
        counter_set_channel_alarm(dev, ALARM_CHANNEL_ID, alarm_cfg);
    }

    // Update device's current time.
    set_current_unix_time(get_current_unix_time() + 1);
}

/* ENABLE_DATETIME_SUBSYSTEM
 * This function sets the soft real-time clock to track the time. It implements the counters
 * with one second alarm to update a global variable. 
 */
int enable_datetime_subsystem() {
    int ret;

    // Set the real time counter to trigger an callback every second.
    const struct device *real_time_counter = DEVICE_DT_GET(RTC_COUNTER_DEVICE);
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

    // Configure the global alarm structure.
    alarm_cfg.flags = 0;
    alarm_cfg.ticks = counter_us_to_ticks(real_time_counter, ALARM_INTERVAL_US);
    alarm_cfg.callback = rtc_isr;
    alarm_cfg.user_data = &alarm_cfg;

    ret = counter_set_channel_alarm(real_time_counter, ALARM_CHANNEL_ID, &alarm_cfg);
    if (ret) {
        LOG_ERR("Failed to set channel alarm (ret %d).", ret);
        return ret;
    }
    LOG_DBG("Channel alarm set successfully.");

    return 0;
}

/* DISABLE_DATETIME_SUBSYSTEM
 * Stops the real-time counter and its alarm to disable the datetime subsystem.
 */
int disable_datetime_subsystem() {
    int ret;

    // Set the real time counter to trigger an callback every second.
    const struct device *real_time_counter = DEVICE_DT_GET(RTC_COUNTER_DEVICE);
    if (!device_is_ready(real_time_counter)) {
        LOG_ERR("Real time counter device is not ready.");
        return -ENODEV;
    }
    LOG_DBG("Real time counter device is ready.");

    // Disable the alarm first.
    reset_alarm = 1;
    LOG_DBG("Reset flag is cleared.");

    // Stop real time counter to track tine.
    ret = counter_stop(real_time_counter);
    if (ret) {
        LOG_ERR("Failed to stop real time counter (ret %d).", ret);
        return ret;
    }
    LOG_DBG("Real time counter stopped successfully.");

    return 0;
}

/* GET_CURRENT_UNIX_TIME
 * Return the UNIX epochs of the current time.
 */
uint32_t get_current_unix_time() {
    device_twin_t *device_twin = get_device_twin_instance();
    return device_twin->unix_time;
}

/* SET_CURRENT_UNIX_TIME
 * Set the current time with UNIX epoch.
 */
int set_current_unix_time(uint32_t new_time) {
    device_twin_t *device_twin = get_device_twin_instance();
    device_twin->unix_time = new_time;
    return 0;
}

/* GET_CURRENT_LOCAL_TIME
 * Return the current time in datetime_t object in local time zone.
 */
datetime_t get_current_local_time(int8_t utc_offset_hours) {
    return unix_to_localtime(get_current_unix_time(), utc_offset_hours);
}

/* UNIX_TO_LOCALTIME
 * Converts Unix time to local time using UTC offset in hours (e.g., +2 or -5)
 */
datetime_t unix_to_localtime(int32_t timestamp, int8_t utc_offset_hours) {
    datetime_t utc;

    // Apply time zone offset
    int32_t adjusted = timestamp + (utc_offset_hours * 3600);

    // Handle negative timestamps (before 1970)
    if (adjusted < 0) {
        // Optional: clamp or handle negative time
        adjusted = 0;
    }

    uint32_t seconds = (uint32_t)adjusted;

    // Time part
    utc.second = seconds % 60;
    seconds /= 60;
    utc.minute = seconds % 60;
    seconds /= 60;
    utc.hour = seconds % 24;
    uint32_t days = seconds / 24;

    utc.weekday = calc_weekday(days);

    // Date part
    uint16_t year = 1970;
    while (1) {
        uint16_t days_in_year = is_leap_year(year) ? 366 : 365;
        if (days < days_in_year) break;
        days -= days_in_year;
        year++;
    }
    utc.year = year;

    uint8_t month = 0;
    while (1) {
        uint16_t dim = days_in_month[month];
        if (month == 1 && is_leap_year(year)) dim++;
        if (days < dim) break;
        days -= dim;
        month++;
    }
    utc.month = month + 1;
    utc.day = days + 1;

    return utc;
}

/* UNIX_TO_UTC
 * Converts Unix time to UTC.
 */ 
datetime_t unix_to_utc(uint32_t timestamp) {
    return unix_to_localtime((int32_t)timestamp, 0);
}

/** **************** **/
/** STATIC FUNCTIONS **/
/** **************** **/

/* IS_LEAP_YEAR
 * Check if the year is a leap year. Internal purposes.
 */
static bool is_leap_year(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* CALC_WEEKDAY
 * Calculate the day of the given days after epoch.
 */
static uint8_t calc_weekday(uint32_t days_since_epoch) {
    return (days_since_epoch + 4) % 7; // 1970-01-01 = Thursday
}
