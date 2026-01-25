#ifndef MCU_MQTT_CLIENT_H
#define MCU_MQTT_CLIENT_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <mqtt_client.h>
#include <esp_event.h>
#include <esp_log.h>
#include "../../shared/cheviot_ip.h"

void mqtt_init (void);

#endif //MCU_MQTT_CLIENT_H