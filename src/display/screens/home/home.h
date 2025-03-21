#ifndef _SCREENS_HOME_HOME_H
#define _SCREENS_HOME_HOME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// The screen object to be used in the display.
extern lv_obj_t *home_screen;

// The init and event functions for the screen.
void home_screen_init();
void home_screen_event(lv_event_t * e);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
