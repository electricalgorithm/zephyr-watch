/** Device Twin implementation for managing synchronized device state.
 *
 * @license: GNU v3
 * @maintainer: electricalgorithm @ github 
 */

#include <stdlib.h>
#include "devicetwin/devicetwin.h"

// Function to construct a single device twin instance. This is a singleton.
static device_twin_t* s_device_twin_instance = NULL;

device_twin_t* get_device_twin_instance(void) {
    return s_device_twin_instance;
}

device_twin_t* create_device_twin_instance(uint32_t unix_time, int8_t utc_zone) {
    device_twin_t* instance = malloc(sizeof(device_twin_t));
    instance->unix_time = unix_time;
    instance->utc_zone = utc_zone;
    // For now, we'll assume the singleton instance is always created.
    s_device_twin_instance = instance;
    return instance;
}
