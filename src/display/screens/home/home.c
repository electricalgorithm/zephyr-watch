/** Home screen implementation for the main smartwatch display.
 * Implements the home screen UI with clock, date, and day display components using LVGL widgets and styling.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#include "lvgl.h"
#include "display/display.h"
#include "display/utils.h"

// Holds the home screen object.
lv_obj_t *home_screen;
lv_obj_t *label_clock;
lv_obj_t *label_date;
lv_obj_t *label_day;

void home_screen_init() {
    // Create the screen object which is the LV object with no parent.
    home_screen = create_screen();

    // Create a vertical flex layout container centered in the screen.
    lv_obj_t *main_column = create_column(home_screen, 100, 100);

    // Create a horizontal flex layout containers that will hold date and time labels.
    lv_obj_t *clock_label_row = create_row(main_column, 100, 20);
    lv_obj_t *date_day_row = create_row(main_column, 100, 20);

    // Add spacing between rows.
    lv_obj_set_style_pad_row(main_column, 5, LV_PART_MAIN);

    // Render items in the screen.
    render_clock_label(clock_label_row);
    render_date_label(date_day_row);
    render_day_label(date_day_row);

    // Add an event handler for all possible events.
    lv_obj_add_event_cb(home_screen, home_screen_event, LV_EVENT_ALL, NULL);
}

void home_screen_event(lv_event_t * e) {
    // lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
}

void render_clock_label(lv_obj_t *flex_element) {
    label_clock = lv_label_create(flex_element);
    lv_obj_set_width(label_clock, LV_SIZE_CONTENT);
    lv_obj_set_height(label_clock, LV_SIZE_CONTENT);
    lv_label_set_text(label_clock, "HH:mm");

    lv_obj_set_style_text_letter_space(label_clock, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(label_clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label_clock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_decor(label_clock, LV_TEXT_DECOR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_46, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void render_date_label(lv_obj_t *flex_element) {
    label_date = lv_label_create(flex_element);
    lv_label_set_text(label_date, "YYYY-MM-DD");
    lv_obj_set_style_text_font(label_date, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void render_day_label(lv_obj_t *flex_element) {
    label_day = lv_label_create(flex_element);
    lv_label_set_text(label_day, "DAY");
    lv_obj_set_style_text_font(label_day, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
}

uint8_t home_screen_set_clock(uint8_t hour, uint8_t minute) {
    // Check if the label_clock is NULL.
    if (label_clock == NULL) return 1;
    // Set the text of the label_clock to the current time in 24-hour format.
    lv_label_set_text_fmt(label_clock, "%02d:%02d", hour, minute);
    // Update the display.
    lv_disp_flush_ready(lv_disp_get_default());
    return 0;
}

uint8_t home_screen_set_date(uint16_t year, uint8_t month, uint8_t day) {
    // Check if the label_date is NULL.
    if (label_date == NULL) return 1;
    // Set the text of the label_date to the current date in "YYYY-MM-DD" format.
    lv_label_set_text_fmt(label_date, "%04u-%02d-%02d", year,  month, day);
    // Update the display.
    lv_disp_flush_ready(lv_disp_get_default());
    return 0;
}

uint8_t home_screen_set_day(const char* day) {
    // Check if the label_clock is NULL.
    if (label_day == NULL) return 1;
    // Set the text of the label_day to the 3 character day name.
    lv_label_set_text(label_day, day);
    // Update the display.
    lv_disp_flush_ready(lv_disp_get_default());
    return 0;
}