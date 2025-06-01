#include "lvgl.h"

void remove_scrollable(lv_obj_t *obj) {
    // Remove the ability to scroll the object.
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
    // Set the scrollbar mode of the object to off.
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
}

lv_obj_t* create_screen() {
    // Create the screen object which is the LV object with no parent.
    lv_obj_t *screen = lv_obj_create(NULL);
    remove_scrollable(screen);
    return screen;
}

lv_obj_t* create_column(lv_obj_t* root, uint8_t width_perc, uint8_t height_perc) {
    // Check the parameters if null. If so, set them 100.
    if (!width_perc) width_perc = 100;
    if (!height_perc) height_perc = 100;

    // Create a vertical flex layout container centered in the screen.
    lv_obj_t *column = lv_obj_create(root);
    lv_obj_set_size(column, LV_PCT(width_perc), LV_PCT(height_perc));
    lv_obj_center(column);

    // Set the layout.
    lv_obj_set_layout(column, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(column, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(column,
                          LV_FLEX_ALIGN_CENTER,  // Main axis (vertical)
                          LV_FLEX_ALIGN_CENTER,  // Cross axis (horizontal)
                          LV_FLEX_ALIGN_CENTER); // Track alignment
    
    // Remove scrolling and set it to off.
    remove_scrollable(column);

    // Remove the borders.
    lv_obj_set_style_border_width(column, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(column, LV_OPA_TRANSP, LV_PART_MAIN);

    // Return the column instance.
    return column;
}

lv_obj_t* create_row(lv_obj_t* root, uint8_t width_perc, uint8_t height_perc) {
    // Check the parameters if null. If so, set them 100.
    if (!width_perc) width_perc = 100;
    if (!height_perc) height_perc = 100;

    // Create a new row object inside the root.
    lv_obj_t *row = lv_obj_create(root);
    lv_obj_set_size(row, LV_PCT(width_perc), LV_PCT(height_perc));
    lv_obj_center(row);

    // Use flext layout on the row.
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row,
                          LV_FLEX_ALIGN_CENTER,  // Main axis (vertical)
                          LV_FLEX_ALIGN_CENTER,  // Cross axis (horizontal)
                          LV_FLEX_ALIGN_CENTER); // Track alignment

    // Remove scrolling and set it to off.
    remove_scrollable(row);

    // Remove the borders.
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);

    // Return the created row object.
    return row;
}