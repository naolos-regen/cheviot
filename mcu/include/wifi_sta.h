#ifndef MCU_WIFI_STA_H
#define MCU_WIFI_STA_H

#include <esp_wifi.h>
#include <esp_wifi_types_generic.h>
#include <credentials.h>
#include <esp_event_base.h>
#include <esp_log.h>

#define MAX_RETRY 10

#define WIFI_CONNECTED_BIT BIT1
#define WIFI_FAIL_BIT	   BIT0

extern void wifi_init_sta();
extern void event_handler(void *, esp_event_base_t, int32_t, void *);

#endif //MCU_WIFI_STA_H