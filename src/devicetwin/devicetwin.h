/** Device Twin interface for managing synchronized device state.
 *
 * @license: GNU v3
 * @maintainer: electricalgorithm @ github 
 */


#ifndef _DEVICE_TWIN_H
#define _DEVICE_TWIN_H

#include "datetime/datetime.h"

#ifdef __cplusplus
extern "C" {
#endif

// Definitions
#define MAX_NOTIFICATION_COUNT 10

/** NotificationCategory represents the category of a notification. */
typedef enum {
    MESSAGE,    // Represents a normal notification. Nothing urgent.
    ALERT       // Represents an urgent notification requiring immediate attention.
} NotificationCategory;

/** NotificationItem struct holds the information regarding to a notification recevied.
 * It is mainly used in receving smartphone notifications in the smartwatch. The system isolates
 * the local notification storage for efficient processing and retrieval.
 * This struct implements the items and an array holds them all.
 */
typedef struct NotificationItem {
    uint32_t id;
    char sender[50];
    char title[50];
    char message[200];
    uint32_t timestamp;
    NotificationCategory category;
} NotificationItem;

/* A struct that holds the device settings.
 * This datatype will hold the neccesarry details
 * about the smart-watch's daily usage such as
 * health monitoring, time, user settings, etc.
 */
typedef struct {
    uint32_t unix_time;
    int8_t utc_zone;
    NotificationItem notifications[MAX_NOTIFICATION_COUNT];
} device_twin_t;

/*
 * Function to construct a new device twin instance with given parameters.
 */
device_twin_t* create_device_twin_instance(uint32_t unix_time, int8_t utc_zone);

/*
 * Factory method to get the existing or newly constructed singleton device twin instance.
 */
device_twin_t* get_device_twin_instance(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif