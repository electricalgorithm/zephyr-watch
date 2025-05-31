#include <stdlib.h>
#include "devicetwin/devicetwin.h"

// Function to construct a single device twin instance. This is a singleton.
static device_twin_t* s_device_twin_instance = NULL;

device_twin_t* get_device_twin_instance(void) {
    return s_device_twin_instance;
}

device_twin_t* create_device_twin_instance(utc_time_t current_time, int8_t utc_zone) {
    device_twin_t* instance = malloc(sizeof(device_twin_t));
    instance->current_time = current_time;
    instance->utc_zone = utc_zone;
    // For now, we'll assume the singleton instance is always created.
    s_device_twin_instance = instance;
    return instance;
}
