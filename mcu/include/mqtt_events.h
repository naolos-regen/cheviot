#ifndef MCU_MQTT_EVENTS_H
#define MCU_MQTT_EVENTS_H

#include <mqtt_ws.h>

#define MQTT_EVENT_LENGTH 11
#define TAG_MQTT_WS "mqtt_events"

extern void (*handler[MQTT_EVENT_LENGTH])	(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);

void mqtt_event_any_handler			(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_error_handler			(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_connected_handler		(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_disconnected_handler		(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_subscribed_handler		(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_unsubscribed_handler		(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_published_handler		(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_data_handler			(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_before_connect_handler		(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_event_deleted_handler			(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);
void mqtt_user_event_handler			(esp_mqtt_event_handle_t, esp_mqtt_client_handle_t, int *);

#endif //MCU_MQTT_EVENTS_H
