#include <mqtt_events.h>

static void
log_error_if_nonzero(const char* msg, const int error_id)
{
	if (error_id != 0)
	{
		ESP_LOGE(TAG_MQTT_WS, "Last error %s: 0x%x", msg, error_id);
	}
}

void (*handler[MQTT_EVENT_LENGTH])
(
	 	esp_mqtt_event_handle_t,
	 	esp_mqtt_client_handle_t,
	 	int *
) = {
	[MQTT_EVENT_ANY+1]				= mqtt_event_any_handler,
	[MQTT_EVENT_ERROR+1]			= mqtt_event_error_handler,
	[MQTT_EVENT_CONNECTED+1]		= mqtt_event_connected_handler,
	[MQTT_EVENT_DISCONNECTED+1]		= mqtt_event_disconnected_handler,
	[MQTT_EVENT_SUBSCRIBED+1]		= mqtt_event_subscribed_handler,
	[MQTT_EVENT_UNSUBSCRIBED+1]		= mqtt_event_unsubscribed_handler,
	[MQTT_EVENT_PUBLISHED+1]		= mqtt_event_published_handler,
	[MQTT_EVENT_DATA+1]				= mqtt_event_data_handler,
	[MQTT_EVENT_BEFORE_CONNECT+1]	= mqtt_event_before_connect_handler,
	[MQTT_EVENT_DELETED+1]			= mqtt_event_deleted_handler,
	[MQTT_USER_EVENT+1]				= mqtt_user_event_handler,
};


void mqtt_event_any_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int* msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event any handler");
}

void mqtt_event_error_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int* msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event error handler");

	if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
	{
		log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
		log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
		log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
		ESP_LOGI(TAG_MQTT_WS, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
	}
}

void mqtt_event_connected_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event connected handler");
	*msg_id = esp_mqtt_client_publish(client, "topic", "data", 0, 1, 0);
	ESP_LOGI(TAG_MQTT_WS, "send publish successful, msg_id=%d", *msg_id);
	*msg_id = esp_mqtt_client_subscribe(client, "topic", *msg_id);
	ESP_LOGI(TAG_MQTT_WS, "send subscribe successful, msg_id=%d", *msg_id);
}

void mqtt_event_disconnected_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event disconnected handler");
}

void mqtt_event_subscribed_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event subscribed handler, msg_id=%d, return code=0x%02x", event->msg_id, (uint8_t)*event->data);
	*msg_id = esp_mqtt_client_publish(client, "topic", "data", 0, 0, 0);
	ESP_LOGI(TAG_MQTT_WS, "send publish successful, msg_id=%d", *msg_id);
}

void mqtt_event_unsubscribed_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event unsubscribed handler, msg_id=%d", event->msg_id);
}

void mqtt_event_published_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event published, msg_id=%d", event->msg_id);
}

void mqtt_event_data_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event data handled, msg_id=%d", event->msg_id);
}

void mqtt_event_before_connect_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event before connect handler (NOP)");
}

void mqtt_event_deleted_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt event deleted handler");
}

void mqtt_user_event_handler(esp_mqtt_event_handle_t event, esp_mqtt_client_handle_t client, int *msg_id)
{
	ESP_LOGI(TAG_MQTT_WS, "mqtt user event handler");
}
