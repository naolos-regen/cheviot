#include <wifi_sta.h>
#include <esp_log.h>
#include <credentials.h>

#include "mqtt_ws.h"

static uint8_t _s_retry_number = 0;

void
event_handler(void * arguments, esp_event_base_t event_base,
				int32_t event_id, void * event_data)
{
	if (WIFI_EVENT == event_base && WIFI_EVENT_STA_START == event_id)
	{
		esp_wifi_connect();
	}
	else if (WIFI_EVENT == event_base && WIFI_EVENT_STA_DISCONNECTED == event_id)
	{
		if (_s_retry_number < MAX_RETRY)
		{
			esp_wifi_connect();
			_s_retry_number++;
		}
	}
	else if (IP_EVENT == event_base && IP_EVENT_STA_GOT_IP == event_id)
	{
		ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *) event_data;
		ESP_LOGI("wifi_sta", "got ip: " IPSTR, IP2STR(&ip_event->ip_info.ip));
		_s_retry_number = 0;

		ESP_LOGI("wifi_sta", "ESP_MQTT_WS");
		mqtt_init();

	}
}

static inline void
esp_event_handler_any_instance(esp_event_handler_instance_t * instance_any_id)
{
	ESP_ERROR_CHECK
	(
		esp_event_handler_instance_register
		(
			WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&event_handler,
			NULL,
			instance_any_id
		)
	);
}

static inline void
esp_event_handler_ip_instance(esp_event_handler_instance_t * instance_got_ip)
{
	ESP_ERROR_CHECK
	(
		esp_event_handler_instance_register
		(
			IP_EVENT,
			IP_EVENT_STA_GOT_IP,
			&event_handler,
			NULL,
			instance_got_ip
		)
	);
}

void
wifi_init_sta(void)
{
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&config));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;

	esp_event_handler_any_instance(&instance_any_id);
	esp_event_handler_ip_instance(&instance_got_ip);

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = WIFI_SSID,
			.password = WIFI_PASS,
			.threshold.authmode = WIFI_AUTH_WPA2_PSK
		}
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
}
