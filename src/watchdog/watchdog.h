/** Watchdog Subsystem for ZephyrWatch.
 * Provides functions to initialize the watchdog and kick it.
 *
 * @license GNU v3
 * @maintainer electricalgorithm @ github
 */

#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the watchdog subsystem. */
int enable_watchdog_subsystem();

/* Deregister the watchdog timers. */
int disable_watchdog_subsystem();

/* Kick the watchdog. */
void kick_watchdog();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
