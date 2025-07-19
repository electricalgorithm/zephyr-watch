/** Current Time Service (CTS) interface for handling time synchronization via Bluetooth GATT.
 * This service allows devices to synchronize their time using the Current Time Service protocol.
 *
 * @license: GNU v3 
 * @maintainer: electricalgorithm @ github
 */

#ifndef CURRENT_TIME_SERVICE_H
#define CURRENT_TIME_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/gatt.h>
#include "timeutils/timeutils.h"

/* Function to update the global unix time - to be implemented in main.c */
extern void update_global_unix_time(uint32_t unix_timestamp);

#ifdef __cplusplus
}
#endif

#endif // CURRENT_TIME_SERVICE_H