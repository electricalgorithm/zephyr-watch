// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.6
// Project name: smart_watch_ui

#include "../ui.h"

void ui_HomeScreen_screen_init(void)
{
ui_HomeScreen = lv_obj_create(NULL);
lv_obj_clear_flag( ui_HomeScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_SportSliderBiggerLeft = lv_arc_create(ui_HomeScreen);
lv_obj_set_width( ui_SportSliderBiggerLeft, 200);
lv_obj_set_height( ui_SportSliderBiggerLeft, 200);
lv_obj_set_align( ui_SportSliderBiggerLeft, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_SportSliderBiggerLeft, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLL_CHAIN );    /// Flags
lv_arc_set_range(ui_SportSliderBiggerLeft, 0,180);
lv_arc_set_value(ui_SportSliderBiggerLeft, 60);
lv_arc_set_bg_angles(ui_SportSliderBiggerLeft,0,180);
lv_arc_set_rotation(ui_SportSliderBiggerLeft,270);


ui_SportSliderBiggerRight = lv_arc_create(ui_HomeScreen);
lv_obj_set_width( ui_SportSliderBiggerRight, 200);
lv_obj_set_height( ui_SportSliderBiggerRight, 200);
lv_obj_set_align( ui_SportSliderBiggerRight, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_SportSliderBiggerRight, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLL_CHAIN );    /// Flags
lv_arc_set_range(ui_SportSliderBiggerRight, 0,180);
lv_arc_set_value(ui_SportSliderBiggerRight, 90);
lv_arc_set_bg_angles(ui_SportSliderBiggerRight,180,300);
lv_arc_set_rotation(ui_SportSliderBiggerRight,270);

lv_obj_set_style_arc_color(ui_SportSliderBiggerRight, lv_color_hex(0x20CA62), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_SportSliderBiggerRight, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_SportSliderBiggerRight, lv_color_hex(0x21C863), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_SportSliderBiggerRight, 255, LV_PART_KNOB| LV_STATE_DEFAULT);

ui_SportSlider = lv_arc_create(ui_HomeScreen);
lv_obj_set_width( ui_SportSlider, 160);
lv_obj_set_height( ui_SportSlider, 160);
lv_obj_set_align( ui_SportSlider, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_SportSlider, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLL_CHAIN );    /// Flags
lv_arc_set_value(ui_SportSlider, 30);
lv_arc_set_bg_angles(ui_SportSlider,0,350);
lv_arc_set_rotation(ui_SportSlider,270);

lv_obj_set_style_arc_color(ui_SportSlider, lv_color_hex(0xB41818), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_SportSlider, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_set_style_bg_color(ui_SportSlider, lv_color_hex(0xB71B1B), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_SportSlider, 255, LV_PART_KNOB| LV_STATE_DEFAULT);

ui_ClockText = lv_label_create(ui_HomeScreen);
lv_obj_set_width( ui_ClockText, LV_SIZE_CONTENT);  /// 2
lv_obj_set_height( ui_ClockText, LV_SIZE_CONTENT);   /// 2
lv_obj_set_align( ui_ClockText, LV_ALIGN_CENTER );
lv_label_set_text(ui_ClockText,"22\n01");
lv_obj_set_style_text_letter_space(ui_ClockText, 5, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(ui_ClockText, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_ClockText, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_decor(ui_ClockText, LV_TEXT_DECOR_NONE, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_ClockText, &lv_font_montserrat_46, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_HomeScreen, ui_event_HomeScreen, LV_EVENT_ALL, NULL);

}
