/** Datetime Subsystem for ZephyrWatch
 * It is a real-time clock implementation based on real-time counters for ESP32.
 * 
 * @license: GNU v3
 * @maintainer: electricalgorithm @ github
*/

#ifndef _DATETIME_H
#define _DATETIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A new datatype to hold easy-to-read datetime. */
typedef struct {
    uint16_t year;   // e.g., 2025
    uint8_t  month;  // 1–12
    uint8_t  day;    // 1–31
    uint8_t  hour;   // 0–23
    uint8_t  minute; // 0–59
    uint8_t  second; // 0–59
    uint8_t  weekday; // 0 = Sunday, ..., 6 = Saturday
} datetime_t;

/* Enables the subsystem to track the real time. */
int enable_datetime_subsystem();

/* Disables the subsystem. */
int disable_datetime_subsystem();

/* Get the current time in UNIX epochs. */
uint32_t get_current_unix_time();

/* Set the current time in UNIX epochs. */
int set_current_unix_time(uint32_t new_time);

/* Get the current time in datetime_t struct in local time zone. */
datetime_t get_current_local_time(int8_t utc_offset_hours);

/* Converts Unix time to local time using datetime_t. */
datetime_t unix_to_localtime(int32_t timestamp, int8_t utc_offset_hours);

/* Converts Unix time to UTC using datetime_t. */
datetime_t unix_to_utc(uint32_t timestamp);

#ifdef __cplusplus
}
#endif

#endif