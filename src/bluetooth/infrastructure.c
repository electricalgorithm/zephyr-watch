/** Bluetooth infrastructure for Zephyr-based devices.
 * This file manages Bluetooth functionality, including advertising and connection handling.
 *
 * @license: GNU v3
 * @maintainer: electricalgorithm @ github 
 */

#include "userinterface/screens/blepairing/blepairing.h"
#include "zephyr/bluetooth/conn.h"
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ZephyrWatch_BLE, LOG_LEVEL_INF);

static const struct bt_data m_ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_CTS_VAL)),
};

static const struct bt_data m_sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static void start_advertisement() {
    const int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, m_ad, ARRAY_SIZE(m_ad), m_sd, ARRAY_SIZE(m_sd));
    if (err) {
        LOG_DBG("Advertising failed to start (err %d).", err);
        return;
    }
    LOG_DBG("Advertising successfully started.");
}

static void process_connection(struct bt_conn *conn, uint8_t err) {
    if (err) LOG_ERR("Connection failed (err %u).", err);
    else {
        char addr[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
        LOG_INF("Connection established to %s.", addr);
    }
}

static void process_disconnection(struct bt_conn *conn, uint8_t reason) {
    LOG_INF("Disconnected (reason 0x%02x).", reason);
}

BT_CONN_CB_DEFINE(connection_callbacks) = {
    .connected = process_connection,
    .disconnected = process_disconnection,
    .recycled = start_advertisement,
};

char* passkey_to_string(const unsigned int passkey) {
    static char passkey_str[7];
    snprintf(passkey_str, sizeof(passkey_str), "%06u", passkey);
    return passkey_str;
}

static void process_passkey_display(struct bt_conn *conn, unsigned int passkey){
    char addr[BT_ADDR_LE_STR_LEN] = {0};
    // Write PIN to the screen.
    blepairing_screen_init();
    blepairing_screen_set_pin(passkey_to_string(passkey));
    blepairing_screen_load();
    LOG_DBG("Displaying passkey on the screen.");

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_DBG("Passkey for %s: %06u", addr, passkey);
}

static void process_auth_cancel(struct bt_conn *conn){
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_DBG("Pairing cancelled: %s", addr);
    blepairing_screen_unload();
}

static void process_pairing_complete(struct bt_conn *conn, bool bonded) {
    LOG_DBG("Pairing complete. Bonded: %s", bonded ? "OK" : "FAILURE");
    blepairing_screen_unload();
}

static void process_pairing_failed(struct bt_conn *conn, enum bt_security_err reason) {
    LOG_DBG("Pairing failed. Reason: 0x%02x", reason);
    bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
    blepairing_screen_unload();
}

static struct bt_conn_auth_info_cb auth_info_callbacks = {
    .pairing_complete = process_pairing_complete,
    .pairing_failed = process_pairing_failed,
};

static struct bt_conn_auth_cb auth_callbacks = {
    .passkey_display = process_passkey_display,
    .passkey_entry = NULL,
    .cancel = process_auth_cancel,
};

/* The API function to enable Bluetooth and start advertisement. */
uint8_t enable_bluetooth_subsystem() {
    int err;

    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d).", err);
        return err;
    }
    LOG_DBG("Bluetooth initialized.");

    err = bt_unpair(BT_ID_DEFAULT, BT_ADDR_LE_ANY);
    if (err) {
        LOG_ERR("Unpairing failed (err %d).", err);
    }
    LOG_DBG("Unpairing successful.");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    err = bt_conn_auth_cb_register(&auth_callbacks);
    if (err) {
        LOG_ERR("Failed to register authentication callbacks (err %d).", err);
        return err;
    }
    LOG_DBG("Authentication callback registered successfully.");

    err = bt_conn_auth_info_cb_register(&auth_info_callbacks);
    if (err) {
        LOG_ERR("Failed to register authentication information callbacks (err %d).", err);
        return err;
    }
    LOG_DBG("Authentication information callback registered successfully.");

    start_advertisement();
    return 0;
}

uint8_t disable_bluetooth_subsystem() {
    int err;

    err = bt_le_adv_stop();
    if (err) {
        LOG_ERR("Advertising failed to stop (err %d).", err);
        return err;
    }
    LOG_DBG("Advertising successfully stopped.");

    err = bt_disable();
    if (err) {
        LOG_ERR("Bluetooth failed to disable (err %d).", err);
        return err;
    }
    LOG_DBG("Bluetooth disabled.");
    return 0;
}