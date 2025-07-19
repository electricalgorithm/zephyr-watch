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

/* A struct that holds the device settings.
 * This datatype will hold the neccesarry details
 * about the smart-watch's daily usage such as
 * health monitoring, time, user settings, etc.
 */
typedef struct {
    datetime_t current_time;
    int8_t utc_zone;
} device_twin_t;

/*
 * Function to construct a new device twin instance with given parameters.
 */
device_twin_t* create_device_twin_instance(datetime_t current_timei, int8_t utc_zone);

/*
 * Factory method to get the existing or newly constructed singleton device twin instance.
 */
device_twin_t* get_device_twin_instance(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif