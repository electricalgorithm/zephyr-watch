#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ZephyrWatch_BluetoothInfra, LOG_LEVEL_INF);

static const struct bt_data m_ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_CTS_VAL)),
};

static const struct bt_data m_sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static void m_connected_callback(struct bt_conn *conn, uint8_t err) {
	if (err) LOG_ERR("Connection failed (err %u).", err);
	else {
		char addr[BT_ADDR_LE_STR_LEN];
		bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
		LOG_INF("Connection established to %s.", addr);
	}
}

static void m_disconnected_callback(struct bt_conn *conn, uint8_t reason) {
	LOG_INF("Disconnected (reason 0x%02x).", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = m_connected_callback,
	.disconnected = m_disconnected_callback,
};

/* The API function to enable Bluetooth and start advertisement. */
uint8_t enable_bluetooth_and_start_advertisement() {
	int err;
	
    err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d).", err);
		return err;
	}

	LOG_INF("Bluetooth initialized.");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN, m_ad, ARRAY_SIZE(m_ad), m_sd, ARRAY_SIZE(m_sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d).", err);
		return err;
	}

	LOG_INF("Advertising successfully started.");
    return 0;
}

uint8_t disable_bluetooth_and_stop_advertisement() {
	int err;

	err = bt_le_adv_stop();
	if (err) {
		LOG_ERR("Advertising failed to stop (err %d).", err);
		return err;
	}

	LOG_INF("Advertising successfully stopped.");

	err = bt_disable();
	if (err) {
		LOG_ERR("Bluetooth failed to disable (err %d).", err);
		return err;
	}

	LOG_INF("Bluetooth disabled.");
	return 0;
}