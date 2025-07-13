#ifndef _DISPLAY_STYLES_WIDGETSTYLE_H
#define _DISPLAY_STYLES_WIDGETSTYLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// Initializes shared widget styles (must be called once at startup).
void widget_style_init();

// Returns a pointer to a style without borders, for clean widget appearance
const lv_style_t* get_widget_style_no_border();

#ifdef __cplusplus
}
#endif

#endif // _DISPLAY_STYLES_WIDGETSTYLE_H
