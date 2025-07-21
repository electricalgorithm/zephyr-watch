/** Menu Screen Interface.
 * Provides functionality to construct a menu using LVGL.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#ifndef _UI_SCREENS_MENU_H
#define _UI_SCREENS_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/* The screen object to be used in the userinterface. */
extern lv_obj_t *menu_screen;

/* The init implementation for the menu screen. */
void menu_screen_init();

/* Event handler for menu screen gestures. It is used to detect non-list events. */
void menu_screen_event(lv_event_t * event);

/**
 * Register an application to be displayed in the menu
 * @param screen The screen object for the application
 * @param name The name of the application to display
 * @return 0 on success, -1 if maximum applications reached
 */
int register_application(lv_obj_t *screen, char *name);

#ifdef __cplusplus
} // extern "C"
#endif

#endif