#include "timeutils/timeutils.h"

static const uint16_t days_in_month[] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
};

static bool is_leap_year(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static uint8_t calc_weekday(uint32_t days_since_epoch) {
    return (days_since_epoch + 4) % 7; // 1970-01-01 = Thursday
}

utc_time_t unix_to_utc(uint32_t timestamp) {
    return unix_to_localtime((int32_t)timestamp, 0);
}

utc_time_t unix_to_localtime(int32_t timestamp, int8_t utc_offset_hours) {
    utc_time_t utc;

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
