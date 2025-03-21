#include "lvgl.h"
#include "display/display.h"

// Holds the home screen object.
lv_obj_t *home_screen;


void home_screen_init() {
    // Create the screen object which is the LV object with no parent.
    home_screen = lv_obj_create(NULL);
    lv_obj_remove_flag(home_screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *ui_ClockText = lv_label_create(home_screen);
    lv_obj_set_width( ui_ClockText, LV_SIZE_CONTENT);  /// 2
    lv_obj_set_height( ui_ClockText, LV_SIZE_CONTENT);   /// 2
    lv_obj_set_align( ui_ClockText, LV_ALIGN_CENTER );
    lv_label_set_text(ui_ClockText,"21\n14");
    lv_obj_set_style_text_letter_space(ui_ClockText, 5, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui_ClockText, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_ClockText, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_text_decor(ui_ClockText, LV_TEXT_DECOR_NONE, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_ClockText, &lv_font_montserrat_46, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_add_event_cb(home_screen, home_screen_event, LV_EVENT_ALL, NULL);
}

void home_screen_event(lv_event_t * e) {
    // lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
}
