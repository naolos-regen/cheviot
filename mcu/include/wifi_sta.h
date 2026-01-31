#ifndef MCU_WIFI_STA_H
#define MCU_WIFI_STA_H

#include <esp_wifi.h>
#include <esp_event_base.h>

#define MAX_RETRY 10

extern void event_handler(void *, esp_event_base_t, int32_t, void *);
extern void esp_event_handler_any_instance(esp_event_handler_instance_t *);
extern void esp_event_handler_ip_instance(esp_event_handler_instance_t *);

extern void wifi_init_sta(void);

#endif //MCU_WIFI_STA_H