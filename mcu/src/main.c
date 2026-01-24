#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_event.h>
#include "esp_netif_ip_addr.h"
#include "esp_system.h"
#include "esp_netif.h"
#include <esp_wifi.h>
#include <wifi_provisioning/manager.h>
#include <mqtt_client.h>
#include "../include/credentials.h"

static const char* TAG = "MQTT_EXAMPLE";

static void log_error_if_nonzero(const char* message, int error_code)
{
	if (error_code != 0)
	{
		ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
	}
}

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		esp_wifi_connect();
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		esp_wifi_connect();
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
		char ip_str[IP4ADDR_STRLEN_MAX];
		esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
		printf("Got IP %s\n", ip_str);
	}
}

static void
wifi_init(void)
{
	esp_err_t ret;

	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = WIFI_SSID,
			.password = WIFI_PASS,
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,
		},
	};
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	ESP_ERROR_CHECK(esp_wifi_start());
}

// TODO: turn that switch to array that can be accessed via event_id index
static void
mqtt_event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", event_base, event_id);
	esp_mqtt_event_handle_t ev = event_data;
	esp_mqtt_client_handle_t cl = ev->client;
	int msg_id;

	switch ((esp_mqtt_event_id_t)event_id)
	{
	case MQTT_EVENT_CONNECTED:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		msg_id = esp_mqtt_client_publish(cl, "test", "data_3", 0, 1, 0);
		ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
		msg_id = esp_mqtt_client_subscribe(cl, "test", 0);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
	}
	break;
	case MQTT_EVENT_DISCONNECTED:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
	}
	break;
	case MQTT_EVENT_SUBSCRIBED:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d return code=0x%02x", ev->msg_id, (uint8_t)*ev->data);
	}
	break;
	case MQTT_EVENT_UNSUBSCRIBED:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", ev->msg_id);
	}
	break;
	case MQTT_EVENT_PUBLISHED:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", ev->msg_id);
	}
	break;
	case MQTT_EVENT_DATA:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		printf("Topic=%.*s\r\n", ev->topic_len, ev->topic);
		printf("Data=%.*s\r\n", ev->data_len, ev->data);
	}
	break;
	case MQTT_EVENT_ERROR:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");

		if (ev->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
		{
			log_error_if_nonzero("reported from esp-tls", ev->error_handle->esp_tls_last_esp_err);
			log_error_if_nonzero("reported from tls stack", ev->error_handle->esp_tls_stack_err);
			log_error_if_nonzero("captured as transport's socket errno", ev->error_handle->esp_transport_sock_errno);
			ESP_LOGI(TAG, "Last errno string (%s)", strerror(ev->error_handle->esp_transport_sock_errno));
		}
	}
	break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", ev->event_id);
		break;
	}
}

void
app_main(void)
{
	ESP_LOGI(TAG, "[APP] startup...");
	ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 "bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
	esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
	esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
	esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
	esp_log_level_set("transport", ESP_LOG_VERBOSE);
	esp_log_level_set("outbox", ESP_LOG_VERBOSE);

	wifi_init();
	esp_mqtt_client_config_t mqtt_confit =
	{
		.broker = {
			.address = {
				.hostname = "192.168.1.3",
				.port = 1883
			}
		}
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_confit);
	esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
	esp_mqtt_client_start(client);
}
