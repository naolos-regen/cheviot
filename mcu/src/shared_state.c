#include <shared_state.h>

#include <mqtt_ws.h>
#include <wifi_sta.h>

#include "credentials.h"

	
static inline wifi_init_config_t
initialize_wifi_sta()
{
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&config));
	

	return config;
}


static struct shared_state 
initialize_state()
{
	struct shared_state state2 = 
	{
		.wifi_cfg = initialize_wifi_sta(),
		.wifi_config = (wifi_config_t)
		{
			.sta = 
			{
				.ssid = WIFI_SSID,
				.password = WIFI_PASS,
				.threshold.authmode = WIFI_AUTH_WPA2_PSK
			}
		},
		.mqtt_config = 
		{
			.broker.address.uri = "mqtt://192.168.1.3"
		},
		.retry_number = 0,
		.mqtt_connected = false,
		.wifi_connected = false,
	};
	return (state2);
}

struct shared_state state;

void 
init_shared_state (void)
{
	state = initialize_state();
	
	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;

	esp_event_handler_any_instance(&instance_any_id);
	esp_event_handler_ip_instance(&instance_got_ip);

	state.mqtt_client = esp_mqtt_client_init(&state.mqtt_config);
	register_client_event(&state.mqtt_client);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &state.wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
}

