/** Bluetooth Subsystem for ZephyrWatch
 * This interface manages Bluetooth functionality, including advertising and connection handling.
 *
 * @license: GNU v3
 * @maintainer: electricalgorithm @ github 
 */

#ifndef BLUETOOTH_INFRASTRUCTURE_H_
#define BLUETOOTH_INFRASTRUCTURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

uint8_t enable_bluetooth_and_start_advertisement();
uint8_t disable_bluetooth_and_stop_advertisement();

#ifdef __cplusplus
}
#endif

#endif /* BLUETOOTH_INFRASTRUCTURE_H_ */