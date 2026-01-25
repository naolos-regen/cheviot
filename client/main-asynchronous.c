#include <string.h>
#include <stdio.h>

#include "include/MQTTAsync.h"
#include "../shared/broker_vars.h"


void set_will_opts(MQTTAsync_willOptions *will_opts)
{
	will_opts->message = "you-must-live";
	will_opts->qos = 1;
	will_opts->retained = 0;
	will_opts->topicName = "staying-alive";

	return;
}

void callback_on_connect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	/* TODO: This shall connect to the topics we will decide on. */
	// MQTTAsync_subscribe(client, "#", 2, NULL);
}

void callback_on_failure(void* context,  MQTTAsync_failureData* res)
{
	printf("failure: %s\n", res->message);
}

void set_con_opts(MQTTAsync_connectOptions *con_opts)
{
	con_opts->keepAliveInterval = 20;
	con_opts->cleansession = 1;
	// con_opts->username = "testuser";
	// con_opts->password = "testpassword";
	con_opts->MQTTVersion = MQTTVERSION_DEFAULT;

	con_opts->onSuccess = &callback_on_connect;
	con_opts->onFailure = &callback_on_failure;

	return;
}

void callback_con_lost(void* context, char* cause)
{
	printf("F: Lost connection. reason: %s\n", cause);
}

int callback_msg_arr(void* context, char* topic_name, int topic_len, MQTTAsync_message* msg)
{
	/* TODO: This shall use libnotify */
	printf("topic_name:'%.*s', msg: '%.*s'", topic_len, topic_name, msg->payloadlen, (char*)msg->payload);
	MQTTAsync_freeMessage(&msg);
	MQTTAsync_free(topic_name);

	return 1;
}

void set_callbacks(MQTTAsync client)
{
	MQTTAsync_connectionLost *con_lost = &callback_con_lost;
	MQTTAsync_messageArrived *msg_arr = &callback_msg_arr;
	MQTTAsync_deliveryComplete *del_cmpl = NULL; /* This isn't needed for our case. */

	MQTTAsync_setCallbacks(
		client,
		client, /* Will be passed to callback functions */
		con_lost,
		msg_arr,
		del_cmpl
	);
}

int main(void)
{
	MQTTAsync client;
	MQTTAsync_connectOptions con_opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_willOptions will_opts = MQTTAsync_willOptions_initializer;
	const char *client_id = "just-monika";
	int res = 0;

	char ip_path[256];
	memset(ip_path, 0, 256);
	sprintf(ip_path, "%s://%s:%s", "tcp", BROKER_IP, BROKER_PORT);

	res = MQTTAsync_create(&client, ip_path, client_id, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);

	printf("%s(): MQTTAsync_create %s.\n", __func__, res == MQTTASYNC_SUCCESS ? "succeeded" : "failed");
	if(res != MQTTASYNC_SUCCESS) {
		MQTTAsync_destroy(&client);
		return 1;
	}

	set_will_opts(&will_opts);
	con_opts.will = &will_opts;
	set_con_opts(&con_opts);
	con_opts.context = client;

	set_callbacks(client);

	res = MQTTAsync_connect(client, &con_opts);
	printf("%s(): MQTTAsync_connect %s. res: %d\n", __func__, res == MQTTASYNC_SUCCESS ? "succeeded" : "failed", res);
	if(res != MQTTASYNC_SUCCESS) {
		MQTTAsync_destroy(&client);
		return 1;
	}

	char ch;
	printf("waitin...\n");
	do
	{
		ch = getchar();
	} while (ch!='Q' && ch != 'q');

	MQTTAsync_destroy(&client);
	return printf("Just Monika\n");
}
