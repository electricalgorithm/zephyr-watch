#ifndef _DISPLAY_UTILS_H
#define _DISPLAY_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/**
 * Create a new screen object with no parent and scrolling.
 * @return The created lv_obj_t screen instance.
 */
lv_obj_t* create_screen();

/**
 * Create a new flex column object inside the root.
 * @param root The parent of the created lv_obj_t column instance.
 * @param width_perc The percentage of the screen width to use for this column.
 * @param height_perc The percentage of the screen height to use for this column.
 * @return The created lv_obj_t column instance.
 */
lv_obj_t* create_column(lv_obj_t* root, uint8_t width_perc, uint8_t height_perc);

/**
 * Create a new flex row object inside the root.
 * @param root The parent of the created lv_obj_t row instance.
 * @param width_perc The percentage of the screen width to use for this row.
 * @param height_perc The percentage of the screen height to use for this row.
 * @return The created lv_obj_t row instance.
 */
lv_obj_t* create_row(lv_obj_t* root, uint8_t width_perc, uint8_t height_perc);


#ifdef __cplusplus
}
#endif

#endif // _DISPLAY_UTILS_H