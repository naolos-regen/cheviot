#include <mqtt_ws.h>
#include <mqtt_events.h>
#include <esp_log.h>

#define TAG "mqtt_ws"

static void
mqtt_event_handler
(
	void *handler_args,
	esp_event_base_t event_base,
	int32_t event_id,
	void *event_data
)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;
	handler[((esp_mqtt_event_id_t) event_id) + 1](event, client, &msg_id);
}

static inline void
register_client_event(esp_mqtt_client_handle_t client)
{
	ESP_ERROR_CHECK
	(
		esp_mqtt_client_register_event
		(
			client,
			ESP_EVENT_ANY_ID,
			mqtt_event_handler,
			NULL
		)
	);
}

void
mqtt_init(void)
{
	const esp_mqtt_client_config_t mqtt_config =
	{
		.broker.address.uri = "mqtt://192.168.1.3",
	};
	ESP_LOGI(TAG, "initializing mqtt client");
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_config);
	register_client_event(client);

	int msg_id;
	msg_id = esp_mqtt_client_enqueue(client, "test", "data", 0, 1, 0, true);
	ESP_LOGI(TAG, "MQTT Client id=%d", msg_id);
	msg_id = esp_mqtt_client_enqueue(client, "test", "Cheviot Sheep", 0, 1, 0, true);
	ESP_LOGI(TAG, "MQTT Client id=%d", msg_id);
	esp_mqtt_client_start(client);
}

