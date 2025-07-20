/** Menu Screen Implementation.
 * Provides functionality to construct a menu using LVGL.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

 #include <zephyr/logging/log.h>
#include "lvgl.h"

#include "misc/lv_event.h"
#include "userinterface/userinterface.h"
#include "userinterface/utils.h"
#include "userinterface/screens/home/home.h"

// Define the maximum number of applications allowed.
#define MAX_APPLICATIONS 10

// Create a logger.
LOG_MODULE_REGISTER(ZephyrWatch_UI_Menu, LOG_LEVEL_INF);

// Structure to hold application information
typedef struct {
    lv_obj_t *screen;
    char *name;
    bool is_registered;
} application_t;

// Array to store registered applications
static application_t applications[MAX_APPLICATIONS];
static uint8_t application_count = 0;

// The screen container.
lv_obj_t *menu_screen;
static lv_obj_t *menu_list;

/**
 * Register an application to be displayed in the menu
 * @param screen The screen object for the application
 * @param name The name of the application to display
 * @return 0 on success, -1 if maximum applications reached
 */
int register_application(lv_obj_t *screen, char *name) {
    if (application_count >= MAX_APPLICATIONS) {
        LOG_ERR("Maximum number of applications reached.");
        return -1; // Maximum applications reached
    }

    applications[application_count].screen = screen;
    applications[application_count].name = name;
    applications[application_count].is_registered = true;
    application_count++;
    LOG_DBG("Application registered successfully: %s", name);
    return 0;
}

/* MENU_SCREEN_EVENT
 * Event handler for menu screen gestures. It is used to detect non-list events.
 */
void menu_screen_event(lv_event_t * event) {
    lv_event_code_t event_code = lv_event_get_code(event);
    // If double clicked, return to home with slide back effect..
    if (event_code == LV_EVENT_DOUBLE_CLICKED) {
        // Home screen is never deleted, but check it for any case. 
        if (!lv_obj_is_valid(home_screen)) {
            home_screen_init();
        }
        // Go back to home screen with slide down animation.
        // Do not delete the screen since register_application will generate new items.
        lv_screen_load_anim(home_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, false);
    }
}

/* MENU_ITEM_EVENT_HANDLER
 * Event handler for menu item clicks. It is the main logic where we start screens.
 */
static void menu_item_event_handler(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    LOG_DBG("Menu-Items - Event code: %d", code);

    if (code == LV_EVENT_CLICKED) {
        uint8_t *app_index = (uint8_t*)lv_event_get_user_data(event);
        LOG_DBG("Clicked app index: %u", *app_index);

        if (*app_index < application_count && applications[*app_index].screen != NULL) {
            // Switch to the selected application screen
            LOG_DBG("Switching to application: %s", applications[*app_index].name);
            lv_screen_load(applications[*app_index].screen);
        }
    }
}

/* CREATE_MENU_ITEM
 * Create one menu item in a list. Each menu item corresponds to an application and presented
 * as buttons.
 */
static void create_menu_item(lv_obj_t *parent, const char *app_name, uint32_t app_index) {
    // Create a button for the menu item
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_width(btn, lv_pct(95));
    lv_obj_set_height(btn, 45);

    // Remove centering to let flex layout handle positioning
    lv_obj_set_align(btn, LV_ALIGN_CENTER);

    // Style the button
    lv_obj_set_style_radius(btn, 10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2E2E2E), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x555555), LV_PART_MAIN);

    // Add hover effect
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_PRESSED);

    // Create label for the button
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, app_name);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(label);

    // Add event handler with application index as user data.
    static uint32_t indices[MAX_APPLICATIONS];
    indices[app_index] = app_index;
    lv_obj_add_event_cb(btn, menu_item_event_handler, LV_EVENT_CLICKED, &indices[app_index]);
    LOG_DBG("Menu item created for app index: %d, app name: %s.", app_index, app_name);
}

/* RENDER_MENU_ITEMS
 * Render all registered applications as menu items.
 */
static void render_menu_items() {
    for (uint8_t i = 0; i < application_count; i++) {
        if (applications[i].is_registered) {
            create_menu_item(menu_list, applications[i].name, i);
        }
    }
}

/* MENU_SCREEN_INIT 
 * Create the menu screen using LVGL definitions.
 */
void menu_screen_init() {
    // Create the screen object which is the LV object with no parent.
    menu_screen = create_screen();
    
    // Create a vertical flex layout container centered in the screen.
    lv_obj_t *main_column = create_column(menu_screen, 100, 100);

    // Create a title row
    lv_obj_t *title_row = create_row(main_column, 100, 15);
    lv_obj_t *title_label = lv_label_create(title_row);
    lv_label_set_text(title_label, "Menu");
    lv_obj_set_style_text_color(title_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_center(title_label);

    // Create a scrollable list container for menu items
    menu_list = create_column(main_column, 100, 80);
    lv_obj_set_style_pad_all(menu_list, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_row(menu_list, 8, LV_PART_MAIN);

    // Enable scrolling for the menu list
    lv_obj_set_scroll_dir(menu_list, LV_DIR_VER);
    lv_obj_set_style_bg_opa(menu_list, LV_OPA_TRANSP, LV_PART_MAIN);

    // Configure scrolling behavior more explicitly
    lv_obj_set_scroll_snap_y(menu_list, LV_SCROLL_SNAP_NONE);
    lv_obj_set_scrollbar_mode(menu_list, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_remove_flag(menu_list, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_add_flag(menu_list, LV_OBJ_FLAG_SCROLLABLE);

    // Set flex layout to column with proper alignment
    lv_obj_set_flex_flow(menu_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Register some example applications
    register_application(NULL, "Settings");
    register_application(NULL, "Stopwatch");
    register_application(NULL, "Weather");
    register_application(NULL, "Music");

    // Render all registered applications
    render_menu_items();

    // Add spacing between elements
    lv_obj_set_style_pad_row(main_column, 5, LV_PART_MAIN);

    // Add gesture detection only to the title area for going back to home screen
    lv_obj_add_event_cb(title_row, menu_screen_event, LV_EVENT_ALL, NULL);
    LOG_DBG("Menu screen initialized successfully.");
}
