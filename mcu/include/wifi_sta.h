#ifndef MCU_WIFI_STA_H
#define MCU_WIFI_STA_H

#include <esp_wifi.h>
#include <credentials.h>
#include <esp_event_base.h>
#include <esp_log.h>

#define MAX_RETRY 10

extern void wifi_init_sta();
extern void event_handler(void *, esp_event_base_t, int32_t, void *);

#endif //MCU_WIFI_STA_H