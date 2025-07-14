#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/gatt.h>
#include <string.h>

#include "timeutils/timeutils.h"
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

	// Convert UNIX timestamp to UTC time
	utc_time_t utc_time = unix_to_utc(unix_timestamp);
	
	// Get the device twin instance
	device_twin_t *device_twin = get_device_twin_instance();
	if (device_twin == NULL) {
		LOG_ERR("Failed to get device twin instance.");
		return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
	}

	// Update the device twin's current time
	device_twin->current_time = utc_time;
	
	LOG_INF("Current time updated: %04d-%02d-%02d %02d:%02d:%02d", 
		utc_time.year, utc_time.month, utc_time.day,
		utc_time.hour, utc_time.minute, utc_time.second);

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
