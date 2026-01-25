#include <stdio.h>
#include <string.h>

#include "include/MQTTClient.h"
#include "../shared/broker_vars.h"

void set_will_opts(MQTTClient_willOptions *will_opts)
{
	will_opts->message = "you-must-live";
	will_opts->qos = 1;
	will_opts->retained = 0;
	will_opts->topicName = "staying-alive";

	return;
}

void set_con_opts(MQTTClient_connectOptions *con_opts)
{
	con_opts->keepAliveInterval = 20;
	con_opts->cleansession = 1;
	// con_opts->username = "testuser";
	// con_opts->password = "testpassword";
	con_opts->MQTTVersion = MQTTVERSION_DEFAULT;
	con_opts->serverURIs = NULL;
	con_opts->serverURIcount = 0;

	return;
}

int main(void)
{
	MQTTClient client;
	MQTTClient_connectOptions con_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
	const char *client_id = "monika-is-monikawaii";

	int res = 0;
	char ip_path[256];

	memset(ip_path, 0, 256);
	sprintf(ip_path, "%s://%s:%s", "tcp", BROKER_IP, BROKER_PORT);

	res = MQTTClient_create(&client, ip_path, client_id, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);

	if(res != MQTTCLIENT_SUCCESS) {
		MQTTClient_destroy(&client);
		printf("how even, this is just a constructor.\n");
		return 1;
	}

	set_will_opts(&will_opts);
	con_opts.will = &will_opts;
	set_con_opts(&con_opts);

	res = MQTTClient_connect(client, &con_opts);
	if(res != MQTTCLIENT_SUCCESS) {
		MQTTClient_destroy(&client);
		printf("con failed\n");
		return 1;
	}

	res = MQTTClient_subscribe(client, "#", 0);
	if(res != MQTTCLIENT_SUCCESS) {
		MQTTClient_destroy(&client);
		printf("sub failed\n");
		return 1;
	}

	char *topic_name = NULL;
	int topic_len = 1;

	MQTTClient_message* m = NULL;
	int cnt = 100;
	printf("getting msgs\n");
	do
	{
		MQTTClient_receive(client, &topic_name, &topic_len, &m, 2000);
		printf("Message received on topic %s is %.*s.\n", topic_name, m->payloadlen, (char*)(m->payload));
		MQTTClient_free(topic_name);
		MQTTClient_freeMessage(&m);
		--cnt;
	}while(topic_name && cnt > 0);

	res = MQTTClient_unsubscribe(client, "#");
	if(res != MQTTCLIENT_SUCCESS) {
		MQTTClient_destroy(&client);
		printf("unsub failed\n");
		return 1;
	}

	return printf("Just Monika\n");
}
