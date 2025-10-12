/** Settings Screen Implementation.
 * Provides functionality to construct a settings screen using LVGL.
 *
 * @license GNU v3
 * @maintainer YehudaEi @ github
 */

#include "lvgl.h"
#include <zephyr/logging/log.h>

#include "bluetooth/infrastructure.h"
#include "display/display.h"
#include "misc/lv_event.h"
#include "userinterface/screens/menu/menu.h"
#include "userinterface/userinterface.h"
#include "userinterface/utils.h"

// Define the maximum number of applications allowed.
#define MAX_APPLICATIONS 10

// Create a logger.
LOG_MODULE_REGISTER(ZephyrWatch_UI_Settings, LOG_LEVEL_INF);

// The screen container.
lv_obj_t* settings_screen;
static lv_obj_t* slider_brightness;
static lv_obj_t* enable_ble;

/* SETTINGS_SCREEN_EVENT
 * Event handler for menu screen gestures. It is used to detect non-list events.
 */
void settings_screen_event(lv_event_t* event)
{
    lv_event_code_t event_code = lv_event_get_code(event);

    // If double clicked, return to home with slide back effect..
    if (event_code == LV_EVENT_DOUBLE_CLICKED) {
        // Home screen is never deleted, but check it for any case.
        if (!lv_obj_is_valid(menu_screen)) {
            menu_screen_init();
        }
        // Go back to home screen with slide down animation.
        // Do not delete the screen since register_application will generate new items.
        lv_screen_load_anim(menu_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    }
}

void brightness_slider_event(lv_event_t* event)
{
    lv_event_code_t event_code = lv_event_get_code(event);

    if (event_code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t* slider = lv_event_get_target_obj(event);
        uint8_t brightness = (uint8_t)lv_slider_get_value(slider);
        LOG_DBG("Brightness: %d%%", brightness);
        change_brightness(brightness);
    }
}

void ble_checkbox_event(lv_event_t* event)
{
    int ret;
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t* checkbox = lv_event_get_target(event);
        bool checked = lv_obj_has_state(checkbox, LV_STATE_CHECKED);

        if (checked) {
            // Enable Bluetooth
            ret = enable_bluetooth_subsystem();
            if (ret != 0) {
                LOG_ERR("Failed to enable bluethooth (err %d)", ret);
                lv_obj_remove_state(checkbox, LV_STATE_CHECKED);
            } else {
                LOG_INF("Bluetooth enabled.");
            }
        } else {
            // Disable Bluetooth
            ret = disable_bluetooth_subsystem();
            if (ret != 0) {
                LOG_ERR("Failed to disable bluethooth (err %d)", ret);
                lv_obj_add_state(checkbox, LV_STATE_CHECKED);
            } else {
                LOG_INF("Bluetooth disabled.");
            }
        }
    }
}

/* MENU_SCREEN_INIT
 * Create the menu screen using LVGL definitions.
 */
void settings_screen_init()
{
    // Create the screen object which is the LV object with no parent.
    settings_screen = create_screen();

    // Create a vertical flex layout container centered in the screen.
    lv_obj_t* main_column = create_column(settings_screen, 100, 100);

    // Create a title row
    lv_obj_t* title_row = create_row(main_column, 100, 15);
    lv_obj_t* title_label = lv_label_create(title_row);
    lv_label_set_text(title_label, "Settings");
    lv_obj_set_style_text_color(title_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_center(title_label);

    lv_obj_t* brightness_obj = lv_obj_create(main_column);
    lv_obj_set_size(brightness_obj, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(brightness_obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(brightness_obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* lb_brightness = lv_label_create(brightness_obj);
    lv_label_set_text(lb_brightness, "Brightness:");
    slider_brightness = lv_slider_create(brightness_obj);
    lv_obj_set_width(slider_brightness, lv_pct(100));
    lv_slider_set_value(slider_brightness, 100, LV_ANIM_OFF);

    lv_obj_t* ble_obj = lv_obj_create(main_column);
    lv_obj_set_size(ble_obj, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ble_obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ble_obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    enable_ble = lv_checkbox_create(ble_obj);
    lv_checkbox_set_text(enable_ble, "Enable BLE");
    lv_obj_set_width(enable_ble, lv_pct(100));
    lv_obj_add_state(enable_ble, LV_STATE_CHECKED);
    lv_obj_set_style_text_font(enable_ble, &lv_font_montserrat_16, LV_PART_MAIN);

    // Add gesture detection only to the title area for going back to home screen
    lv_obj_add_event_cb(title_row, settings_screen_event, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(slider_brightness, brightness_slider_event, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(enable_ble, ble_checkbox_event, LV_EVENT_VALUE_CHANGED, NULL);
    LOG_DBG("Settings screen initialized successfully.");
}
