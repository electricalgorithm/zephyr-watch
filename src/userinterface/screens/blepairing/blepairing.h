/** BLE Pairing/Bonding Interface.
 * Provides functionality to show an PIN code through UI.
 * This PIN code is used in connecting client device.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#ifndef _UI_SCREENS_BLEPAIRING_H
#define _UI_SCREENS_BLEPAIRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/* The screen object to be used in the userinterface. */
extern lv_obj_t *blepairing_screen;

/* The init implementation for the BLE Pairing screen. */
void blepairing_screen_init();

/** Event handler for BLE screen gestures.
 * @param event The event object containing gesture details.
 * @return void
 */
void blepairing_screen_event(lv_event_t * event);

/** Set the PIN code to be displayed on the BLE pairing screen.
 * @param pin_code A 6-character string representing the PIN code.
 * @return 0 on success, 1 on failure.
 */
uint8_t blepairing_screen_set_pin(const char *pin_code);


/** Load the BLE pairing screen.
 * @param kill_previous A boolean indicating whether to close the previous screen before loading the BLE pairing screen.
 * @return void
 */
void blepairing_screen_load(bool kill_previous);

#ifdef __cplusplus
} // extern "C"
#endif

#endif