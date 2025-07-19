/** Display initialization interface for LVGL-based UI setup.
 * Provides functions to initialize the display system and set up the default theme for the smartwatch interface.
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
#include "display/screens/home/home.h"

void display_init();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
