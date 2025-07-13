#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/gatt.h>

#include "current_time_service.h"

LOG_MODULE_REGISTER(ZephyrWatch_CurrentTimeService, LOG_LEVEL_INF);


/* Holds current time data. */
static uint8_t m_current_time_data[10] = {
	232U,	// Year (2024) last byte
	7U,		// Year (2024) first byte
	5U,		// Month (May)
	13U,	// Day (13th)
	23U,	// Hours (23)
	45U,	// Minutes (45)
	30U,	// Seconds (30)
	2U,		// Day of week (Tuesday)
	0U,		// Fractions 256 part of 'Exact Time 256'
	0U,		// Adjust reason
};

/* Current Time Service Declaration */
static ssize_t m_time_write_callback(
	struct bt_conn *conn,
	const struct bt_gatt_attr *attr,
	const void *buf,
	uint16_t len,
	uint16_t offset,
	uint8_t flags) {

	// Get the value of the attribute.
	uint8_t *value = attr->user_data;

	// Check if the value written is valid.
	if (offset + len > sizeof(m_current_time_data)) {
		LOG_ERR("Write request too long.");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Copy the new value into the m_current_time_data array (given as buffer).
	memcpy(value + offset, buf, len);
	LOG_INF("Current time updated from the Bluetooth client.");
	return len;
}

/* Current Time Service Declaration */
/*
BT_GATT_SERVICE_DEFINE(cts_cvs,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_CTS),
	BT_GATT_CHARACTERISTIC(
		BT_UUID_CTS_CURRENT_TIME,
		BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_WRITE,
		NULL, m_time_write_callback, m_current_time_data),
);
*/

/* API for the Current Time Service application layer consumers.
 * - get_current_time: Get the current time. Returns a struct with
 * proper key-value pairs for year, month, hours, minutes, and seconds.
 * - set_current_time: Set the current time. Takes a struct with
 * proper key-value pairs for year, month, hours, minutes, and seconds.
 */

struct current_time get_current_time(void) {
	struct current_time time;
	time.year = sys_le16_to_cpu(*(uint16_t *) &m_current_time_data[0]);
	time.month = m_current_time_data[2];
	time.day = m_current_time_data[3];
	time.hours = m_current_time_data[4];
	time.minutes = m_current_time_data[5];
	time.seconds = m_current_time_data[6];
	return time;
}

void set_current_time(struct current_time time) {
	*(uint16_t *) &m_current_time_data[0] = sys_cpu_to_le16(time.year);
	m_current_time_data[2] = time.month;
	m_current_time_data[3] = time.day;
	m_current_time_data[4] = time.hours;
	m_current_time_data[5] = time.minutes;
	m_current_time_data[6] = time.seconds;
}