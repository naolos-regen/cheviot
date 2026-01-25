#include <mqtt_ws.h>
#include <mqtt_events.h>

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
	esp_mqtt_event_handle_t event =  (esp_mqtt_event_handle_t) event_data;
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
			.broker.address.hostname = HOSTNAME,
			.broker.address.port	 = PORT_NUM
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_config);
	register_client_event(client);
	esp_mqtt_client_start(client);

}

