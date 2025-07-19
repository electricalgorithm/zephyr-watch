/** Current Time Service (CTS) implementation for handling time synchronization via Bluetooth GATT.
 * This service allows devices to synchronize their time using the Current Time Service protocol.
 *
 * @license: GNU v3 
 * @maintainer: electricalgorithm @ github
 */

#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/gatt.h>
#include <string.h>

#include "current_time_service.h"
#include "datetime/datetime.h"
#include "devicetwin/devicetwin.h"

LOG_MODULE_REGISTER(ZephyrWatch_CurrentTimeService, LOG_LEVEL_INF);

/* Current Time Service Write Callback */
static ssize_t m_time_write_callback(
	struct bt_conn *conn,
	const struct bt_gatt_attr *attr,
	const void *buf,
	uint16_t len,
	uint16_t offset,
	uint8_t flags) {

	// Check if we received exactly 4 bytes for UNIX timestamp
	if (len != 4 || offset != 0) {
		LOG_ERR("Invalid write length or offset. Expected 4 bytes at offset 0, got %d bytes at offset %d", len, offset);
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Extract the UNIX timestamp from the buffer (assuming little-endian)
	uint32_t unix_timestamp = sys_le32_to_cpu(*(uint32_t *)buf);
	LOG_INF("Received UNIX timestamp: %u", unix_timestamp);

	// Get the device twin instance to get the UTC zone
	device_twin_t *device_twin = get_device_twin_instance();
	if (device_twin == NULL) {
		LOG_ERR("Failed to get device twin instance.");
		return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
	}

	// Convert UNIX timestamp to local time using the device's UTC zone
	datetime_t local_time = unix_to_localtime(unix_timestamp, device_twin->utc_zone);

	// Update the device twin's current time with local time
	device_twin->current_time = local_time;
	
	// Update the global unix time used by the UI system
	update_global_unix_time(unix_timestamp);

	LOG_INF("Current time updated to local time: %04d-%02d-%02d %02d:%02d:%02d (UTC%+d)", 
		local_time.year, local_time.month, local_time.day,
		local_time.hour, local_time.minute, local_time.second,
		device_twin->utc_zone);

	return len;
}

/* Dummy data for GATT characteristic - not used for actual data storage */
static uint8_t dummy_data[4] = {0};

/* Current Time Service Declaration */
BT_GATT_SERVICE_DEFINE(cts_cvs,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_CTS),
	BT_GATT_CHARACTERISTIC(
		BT_UUID_CTS_CURRENT_TIME,
		BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_WRITE,
		NULL, m_time_write_callback, dummy_data),
);
