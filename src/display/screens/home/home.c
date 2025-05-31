#include "lvgl.h"
#include "display/display.h"

// Holds the home screen object.
lv_obj_t *home_screen;
lv_obj_t *label_clock;

void home_screen_init() {
    // Create the screen object which is the LV object with no parent.
    home_screen = lv_obj_create(NULL);
    // Remove the ability to scroll the screen.
    lv_obj_remove_flag(home_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Create a new label and expand its size to content size and align it to center.
    label_clock = lv_label_create(home_screen);
    lv_obj_set_width(label_clock, LV_SIZE_CONTENT);
    lv_obj_set_height(label_clock, LV_SIZE_CONTENT);
    lv_obj_set_align(label_clock, LV_ALIGN_CENTER);
    
    // Set the default text.
    lv_label_set_text(label_clock, "HH\nMM");

    // Format the clock text string.
    /* 1. Set the letter spacing to 5. (Between letters)
     * 2. Set the line spacing to 0. (Between new lines)
     * 3. Align the text automatically.
     * 4. Set no decoration on the text.
     * 5. Set the font and the size to Montserrat 46.
     */
    lv_obj_set_style_text_letter_space(label_clock, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(label_clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label_clock, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_decor(label_clock, LV_TEXT_DECOR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_46, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Add an event handler for all possible events.
    lv_obj_add_event_cb(home_screen, home_screen_event, LV_EVENT_ALL, NULL);
}

void home_screen_event(lv_event_t * e) {
    // lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
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
