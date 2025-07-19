/** Display Subsystem for ZephyrWatch.
 * Provides functions to initialize the display system.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int enable_display_subsystem();
int disable_display_subsystem();
int change_brightness(uint8_t perc);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
