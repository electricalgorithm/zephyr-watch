/** BLE Pairing/Bonding Screen Implementation.
 * Provides functionality to display a 6-digit PIN code for Bluetooth pairing/bonding.
 * Features a modern, visually appealing design with animated elements.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#include <zephyr/logging/log.h>
#include <string.h>
#include "lvgl.h"
#include "userinterface/utils.h"
#include "userinterface/screens/home/home.h"
#include "userinterface/screens/blepairing/blepairing.h"

// Create a logger.
LOG_MODULE_REGISTER(ZephyrWatch_UI_BLEPairing, LOG_LEVEL_DBG);

// Forward declarations for static functions
static void render_title_label(lv_obj_t *flex_element);
static void render_instruction_label(lv_obj_t *flex_element);
static void render_pin_display(lv_obj_t *flex_element);
static void render_footer_label(lv_obj_t *flex_element);

// Holds the BLE pairing screen objects.
lv_obj_t *blepairing_screen;
static lv_obj_t *label_title;
static lv_obj_t *label_instruction;
static lv_obj_t *pin_container;
static lv_obj_t *pin_digits[6];  // Array to hold individual PIN digit labels
static lv_obj_t *label_footer;

// Current PIN code (default for demonstration)
static char current_pin[7] = "123456";

void blepairing_screen_event(lv_event_t * event) {
    lv_event_code_t event_code = lv_event_get_code(event);
    LOG_DBG("Event code: %d", event_code);

    // Handle double click to return to home.
    if (event_code == LV_EVENT_DOUBLE_CLICKED) {
        LOG_DBG("Double click detected: returning to home screen.");
        if (!lv_obj_is_valid(home_screen)) {
            home_screen_init();
        }
        lv_screen_load_anim(home_screen, LV_SCR_LOAD_ANIM_FADE_OUT, 300, 0, true);
    }
}

void blepairing_screen_init() {
    LOG_DBG("Initializing BLE pairing screen");

    // Create the screen object which is the LV object with no parent.
    blepairing_screen = create_screen();

    // Create main vertical layout container
    lv_obj_t *main_column = create_column(blepairing_screen, 100, 100);
    lv_obj_set_style_pad_all(main_column, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_row(main_column, 3, LV_PART_MAIN);

    // Create rows for different sections
    lv_obj_t *title_row = create_row(main_column, 100, 15);
    lv_obj_t *instruction_row = create_row(main_column, 100, 15);
    lv_obj_t *pin_row = create_row(main_column, 100, 40);
    lv_obj_t *footer_row = create_row(main_column, 100, 15);

    // Render all components
    render_title_label(title_row);
    render_instruction_label(instruction_row);
    render_pin_display(pin_row);
    render_footer_label(footer_row);

    // Add event handler for gestures
    lv_obj_add_event_cb(blepairing_screen, blepairing_screen_event, LV_EVENT_ALL, NULL);
    LOG_DBG("BLE pairing screen initialized successfully.");
}

static void render_title_label(lv_obj_t *flex_element) {
    label_title = lv_label_create(flex_element);
    lv_label_set_text(label_title, "Pairing Request");

    // Center the title
    lv_obj_set_width(label_title, LV_SIZE_CONTENT);
    lv_obj_set_height(label_title, LV_SIZE_CONTENT);
    lv_obj_set_align(label_title, LV_ALIGN_CENTER);

    // Style the title
    lv_obj_set_style_text_color(label_title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_center(label_title);
}

static void render_instruction_label(lv_obj_t *flex_element) {
    label_instruction = lv_label_create(flex_element);
    lv_label_set_text(label_instruction, "Enter PIN on your device.");

    // Center the instruction
    lv_obj_set_width(label_instruction, LV_SIZE_CONTENT);
    lv_obj_set_height(label_instruction, LV_SIZE_CONTENT);
    lv_obj_set_align(label_instruction, LV_ALIGN_CENTER);

    // Style the instruction
    lv_obj_set_style_text_align(label_instruction, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_instruction, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label_instruction, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void render_pin_display(lv_obj_t *flex_element) {
    // Create a container for the PIN digits with horizontal layout
    pin_container = lv_obj_create(flex_element);
    lv_obj_set_size(pin_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(pin_container, LV_ALIGN_CENTER);

    // Configure flexbox layout for PIN container
    lv_obj_set_layout(pin_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(pin_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(pin_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(pin_container, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(pin_container, 15, LV_PART_MAIN);

    // Remove background and border from container
    lv_obj_set_style_bg_opa(pin_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(pin_container, LV_OPA_TRANSP, LV_PART_MAIN);
    
    // Create individual digit displays
    for (int i = 0; i < 6; i++) {
        // Create a container for each digit
        lv_obj_t *digit_box = lv_obj_create(pin_container);
        lv_obj_set_size(digit_box, 25, 45);

        // Style the digit box with rounded corners and shadow
        lv_obj_set_style_radius(digit_box, 8, LV_PART_MAIN);
        lv_obj_set_style_bg_color(digit_box, lv_color_hex(0x404040), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(digit_box, LV_OPA_100, LV_PART_MAIN);
        lv_obj_set_style_border_color(digit_box, lv_color_hex(0x555555), LV_PART_MAIN);
        lv_obj_set_style_border_width(digit_box, 2, LV_PART_MAIN);
        lv_obj_set_style_border_opa(digit_box, LV_OPA_50, LV_PART_MAIN);

        // Create the digit label
        pin_digits[i] = lv_label_create(digit_box);
        lv_label_set_text_fmt(pin_digits[i], "%c", current_pin[i]);

        // Center the digit in its box
        lv_obj_set_align(pin_digits[i], LV_ALIGN_CENTER);

        // Style the digit text
        lv_obj_set_style_text_font(pin_digits[i], &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(pin_digits[i], lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(pin_digits[i], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

static void render_footer_label(lv_obj_t *flex_element) {
    label_footer = lv_label_create(flex_element);
    lv_label_set_text(label_footer, "Double tap to cancel");

    // Center the footer
    lv_obj_set_width(label_footer, LV_SIZE_CONTENT);
    lv_obj_set_height(label_footer, LV_SIZE_CONTENT);
    lv_obj_set_align(label_footer, LV_ALIGN_CENTER);

    // Style the footer
    lv_obj_set_style_text_align(label_footer, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_footer, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label_footer, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

uint8_t blepairing_screen_set_pin(const char *pin_code) {
    // Validate input
    if (pin_code == NULL || strlen(pin_code) != 6) {
        LOG_ERR("Invalid PIN code provided");
        return 1;
    }

    // Check if PIN digits are valid
    for (int i = 0; i < 6; i++) {
        if (pin_digits[i] == NULL) {
            LOG_ERR("PIN digit %d is NULL", i);
            return 1;
        }
    }
    // Update the PIN code
    strncpy(current_pin, pin_code, 6);
    current_pin[6] = '\0';
    // Update each digit display
    for (int i = 0; i < 6; i++) {
        lv_label_set_text_fmt(pin_digits[i], "%c", current_pin[i]);
    }
    // Update the display
    lv_disp_flush_ready(lv_disp_get_default());
    LOG_DBG("PIN code updated to: %s", current_pin);
    return 0;
}

void blepairing_screen_load(bool kill_previous) {
    if (!lv_obj_is_valid(blepairing_screen)) {
        blepairing_screen_init();
    }
    // Go back to home screen with slide down animation.
    // Do not delete the screen since register_application will generate new items.
    lv_screen_load_anim(blepairing_screen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, kill_previous);
}