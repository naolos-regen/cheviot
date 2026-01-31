#ifndef MCU_MQTT_CLIENT_H
#define MCU_MQTT_CLIENT_H

#include <mqtt_events.h>

extern void mqtt_event_handler (void *, esp_event_base_t, int32_t, void *); 
extern void register_client_event (esp_mqtt_client_handle_t *);

#endif //MCU_MQTT_CLIENT_H
