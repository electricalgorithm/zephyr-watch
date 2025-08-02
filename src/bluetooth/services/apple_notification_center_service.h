/** Apple Notification Center Service (ANCS) interface for handling notification exchange with 
 * iOS devices.
 *
 * @license: GNU v3 
 * @maintainer: electricalgorithm @ github
 */

#ifndef APPLE_NOTIFICATION_CENTER_SERVICE_H
#define APPLE_NOTIFICATION_CENTER_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EventID enumeration for categorizing notification events. */
typedef enum {
    EVENT_ID_NOTIFICATION_ADDED,
    EVENT_ID_NOTIFICATION_MODIFIED,
    EVENT_ID_NOTIFICATION_REMOVED,
    EVENT_ID_RESERVED               // 3–255
} EventID;

/* EventFlags enumeration for defining notification event flags. */
typedef enum {
    EVENT_FLAG_SILENT,          // (1 << 0)
    EVENT_FLAG_IMPORTANT,       // (1 << 1)
    EVENT_FLAG_PRE_EXISTING,    // (1 << 2)
    EVENT_FLAG_POSITIVE_ACTION, // (1 << 3)
    EVENT_FLAG_NEGATIVE_ACTION, // (1 << 4)
    EVENT_FLAG_RESERVED         // (1 << 5)–(1 << 7)
} EventFlags;

/* CategoryID enumeration provides the category of the notificaiton. */
typedef enum {
    CATEGORY_ID_OTHER,
    CATEGORY_ID_INCOMING_CALL,
    CATEGORY_ID_MISSED_CALL,
    CATEGORY_ID_VOICEMAIL,
    CATEGORY_ID_SOCIAL,
    CATEGORY_ID_SCHEDULE,
    CATEGORY_ID_EMAIL,
    CATEGORY_ID_NEWS,
    CATEGORY_ID_HEALTH_AND_FITNESS,
    CATEGORY_ID_BUSINESS_AND_FINANCE,
    CATEGORY_ID_LOCATION,
    CATEGORY_ID_ENTERTAINMENT,
    CATEGORY_ID_RESERVED        // 12-255
} CategoryID;

/** NotificatonSourceData struct defines the data type of the incoming GATT data.
 * It follows the specification: dar.vin/ancs
 */
struct NotificationSourceData {
    uint8_t event_id;          // EventID: Notification Added, Modified, Removed
    uint8_t event_flags;       // EventFlags: Silent, Important, etc.
    uint8_t category_id;       // CategoryID: Incoming Call, Email, etc.
    uint8_t category_count;    // Number of active notifications in the category
    uint32_t notification_uid; // Unique identifier for the notification
};

/* Helper getters and setters for type-safe enum conversions. */
EventID get_event_id(const struct NotificationSourceData *data);
CategoryID get_category_id(const struct NotificationSourceData *data);
void set_event_id(struct NotificationSourceData *data, EventID event_id);
void set_category_id(struct NotificationSourceData *data, CategoryID category_id);

/* Validation Helpers */
bool is_valid_event_id(uint8_t event_id);
bool is_valid_category_id(uint8_t category_id);

/* Data Parser for NotificationSourceData */
bool parse_notification_source_data(const uint8_t *gatt_data, size_t data_len, 
                                   struct NotificationSourceData *notification);

#ifdef __cplusplus
}
#endif

#endif // APPLE_NOTIFICATION_CENTER_SERVICE_H