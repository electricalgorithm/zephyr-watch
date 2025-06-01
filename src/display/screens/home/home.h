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

// Render labels.
void render_clock_label(lv_obj_t *flex_element);
void render_date_label(lv_obj_t *flex_element);
void render_day_label(lv_obj_t *flex_element);

// Set the values on the home-screen. Returns 0 if successful, 1 otherwise.
uint8_t home_screen_set_clock(uint8_t hour, uint8_t minute);
uint8_t home_screen_set_date(uint16_t year, uint8_t month, uint8_t day);
uint8_t home_screen_set_day(const char* day);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
