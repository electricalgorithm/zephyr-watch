#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t year;   // e.g., 2025
    uint8_t  month;  // 1–12
    uint8_t  day;    // 1–31
    uint8_t  hour;   // 0–23
    uint8_t  minute; // 0–59
    uint8_t  second; // 0–59
    uint8_t  weekday; // 0 = Sunday, ..., 6 = Saturday
} utc_time_t;

// Converts Unix time to UTC
utc_time_t unix_to_utc(uint32_t timestamp);

// Converts Unix time to local time using UTC offset in hours (e.g., +2 or -5)
utc_time_t unix_to_localtime(int32_t timestamp, int8_t utc_offset_hours);

#ifdef __cplusplus
}
#endif

#endif // TIMEUTILS_H
