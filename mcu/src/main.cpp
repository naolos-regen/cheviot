#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Ping.h>
#include <Arduino.h>
#include "../include/credentials.h"

const char* ssid = SSID;
const char* password = PASSWORD;

const char* mqtt_server = "192.168.1.3";  
const int mqtt_port = 1883;  

WiFiClient espClient;
PubSubClient client(espClient);

const IPAddress remote_ip(192,168,1,3);

void callback(char* topic, byte* payload, unsigned int length)
{
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++)
        {
                Serial.print((char)payload[i]);
        }
        Serial.println();
}

void reconnect()
{
        while (!client.connected())
        {
                Serial.println("Attempting MQTT connection...");
                if (client.connect("ESP32Client"))
                {
                        Serial.println("Connected to MQTT Broker");
                        client.subscribe("test");

                        client.publish("test", "Hello from ESP32!");
                }
                else
                {
                        Serial.print("Failed, rc=");
                        Serial.print(client.state());
                        Serial.println(" Retrying in 5 seconds");
                        delay(5000);
                }
        }
}

void setup()
{
        Serial.begin(115200);
        delay(10);

        Serial.println();
        Serial.println("Connecting to WiFi");
        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED)
        {
                delay(500);
                Serial.print(".");
        }

        Serial.println("\nWiFi Connected");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        if (Ping.ping(remote_ip))
        {
                Serial.println("Ping to 192.168.1.3 Successful!");
        }
        else
        {
                Serial.println("Ping to 192.168.1.3 Failed :(");
        }

        client.setServer(mqtt_server, mqtt_port);
        client.setCallback(callback);
}

void loop()
{
        if (WiFi.status() != WL_CONNECTED)
        {
                WiFi.reconnect();
        }

        if (!client.connected())
        {
                reconnect();
        }

        client.loop();

        static unsigned long lastPublish = 0;
        if (millis() - lastPublish > 5000)    
        {
                client.publish("test", "Periodic update from ESP32");
                lastPublish = millis();
        }
}
