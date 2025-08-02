/** Apple Notification Center Service (ANCS) implementation for handling notification exchange with 
 * iOS devices.
 *
 * @license: GNU v3 
 * @maintainer: electricalgorithm @ github
 */

#include "bluetooth/services/apple_notification_center_service.h"

EventID get_event_id(const struct NotificationSourceData *data) {
    return (EventID)data->event_id;
}

void set_event_id(struct NotificationSourceData *data, EventID event_id) {
    data->event_id = (uint8_t)event_id;
}

CategoryID get_category_id(const struct NotificationSourceData *data) {
    return (CategoryID)data->category_id;
}

void set_category_id(struct NotificationSourceData *data, CategoryID category_id) {
    data->category_id = (uint8_t)category_id;
}

//
// EventFlags get_event_flags(const struct NotificationSourceData *data) {
//     return (EventFlags)data->event_flags;
// }
//
// void set_event_flags(struct NotificationSourceData *data, EventFlags event_flags) {
//     data->event_flags = (uint8_t)event_flags;
// }
//

bool is_valid_event_id(uint8_t event_id) {
    return event_id < EVENT_ID_RESERVED;
}

bool is_valid_category_id(uint8_t category_id) {
    return category_id < CATEGORY_ID_RESERVED;
}

/* Example usage function for parsing GATT data */
bool parse_notification_source_data(const uint8_t *gatt_data, size_t data_len, 
                                   struct NotificationSourceData *notification) {
    // Check the size of the received data.
    if (data_len < sizeof(struct NotificationSourceData)) {
        return false;
    }

    // Parse it.
    notification->event_id = gatt_data[0];
    notification->event_flags = gatt_data[1];
    notification->category_id = gatt_data[2];
    notification->category_count = gatt_data[3];

    // Notification_uid is 4 bytes (little-endian)
    notification->notification_uid = (uint32_t)gatt_data[4] |
                                   ((uint32_t)gatt_data[5] << 8) |
                                   ((uint32_t)gatt_data[6] << 16) |
                                   ((uint32_t)gatt_data[7] << 24);

    // Validate the parsed data
    return is_valid_event_id(notification->event_id) && 
           is_valid_category_id(notification->category_id);
}