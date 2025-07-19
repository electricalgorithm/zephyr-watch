/** Display initialization interface for LVGL-based UI setup.
 * Sets up the UI and the default theme for the smartwatch interface.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#ifndef _SMART_WATCH_UI_UI_H
#define _SMART_WATCH_UI_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "userinterface/screens/home/home.h"

/* Initilize the user interface. */
void user_interface_init();

/* Refresh/process the user interface jobs. */
void user_interface_task_handler();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
