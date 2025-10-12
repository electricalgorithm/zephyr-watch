/** Settings Screen Interface.
 * Provides functionality to construct a settings screen using LVGL.
 *
 * @license GNU v3
 * @maintainer YehudaEi @ github
 */

#ifndef _UI_SCREENS_SETTINGS_H
#define _UI_SCREENS_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/* The screen object to be used in the userinterface. */
extern lv_obj_t* settings_screen;

/* The init implementation for the settings screen. */
void settings_screen_init();

/* Event handler for settings screen gestures. It is used to detect non-list events. */
void settings_screen_event(lv_event_t* event);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
