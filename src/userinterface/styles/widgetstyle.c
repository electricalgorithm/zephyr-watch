/** Widget styling implementation for LVGL components.
 * Implements shared widget styles including borderless styles for clean UI appearance across the
 * smartwatch interface.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#include "lvgl.h"
#include "userinterface/styles/widgetstyle.h"

static lv_style_t style_no_border;

void widget_style_init() {
    lv_style_init(&style_no_border);

    // Remove borders
    lv_style_set_border_width(&style_no_border, 0);

    // Transparent background
    lv_style_set_bg_opa(&style_no_border, LV_OPA_TRANSP);

    // Remove outline and shadow just in case
    lv_style_set_outline_width(&style_no_border, 0);
    lv_style_set_shadow_width(&style_no_border, 0);

    // Optional: remove padding if undesired
    lv_style_set_pad_all(&style_no_border, 0);
}

const lv_style_t* get_widget_style_no_border() {
    return &style_no_border;
}
