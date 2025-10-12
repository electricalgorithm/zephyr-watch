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

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

int enable_bluetooth_subsystem();
int disable_bluetooth_subsystem();

#ifdef __cplusplus
}
#endif

#endif /* BLUETOOTH_INFRASTRUCTURE_H_ */
