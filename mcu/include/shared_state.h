#ifndef MCU_SHARED_STATE_H
#define MCU_SHARED_STATE_H
#include <esp_wifi.h>
#include <esp_wifi_types_generic.h>
#include <mqtt_client.h>

struct shared_state
{
	wifi_init_config_t wifi_cfg;
	wifi_config_t wifi_config;

	esp_mqtt_client_handle_t mqtt_client;
	esp_mqtt_client_config_t mqtt_config;
	uint8_t retry_number;
	bool mqtt_connected, wifi_connected;
};

extern struct shared_state state;

extern void init_shared_state(void);

#endif
