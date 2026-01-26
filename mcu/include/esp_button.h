#ifndef MCU_ESP_BUTTON_H
#define MCU_ESP_BUTTON_H

#include <esp_event_base.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include <utility>

// 4B sadly
enum button_state
{
	PRESSED			= 0,
	RELEASED		= 1,
};

// 4B
enum button_event_type
{
	BUTTON_UP		= 0,
	BUTTON_DOWN		= 1,
	BUTTON_PRESS		= 2,
	BUTTON_LONG_PRESS	= 3,
	BUTTON_HELD		= 4,
	BUTTON_RELEASE		= 5
};

// 4B + 4B + 1B => 9B
struct button_info
{
	const char *name;
	gpio_num_t pin_number;
	const bool inverted;
};

// 49B
struct button
{
	// 9B
	struct button_info button;

	// 4B * 2 => 8B
	esp_timer_handle_t debounce_timer;
	esp_timer_handle_t held_timer;

	// 4B * 3 => 12B;
	uint32_t press_event_bit;
	uint32_t timer_event_bit;
	uint32_t repeat_event_bit;

	// 4B * 5 => 20B;
	uint32_t debounce;
	uint32_t short_press;
	uint32_t long_press;
	uint32_t hold_press;
	uint32_t release;
};

/* TODO: We need to combine run wifi thread separately of button+mqtt
 *			Scenario: Wi-Fi loses connection, what happens to button+mqtt?
 *			Probably: button+mqtt should collect the messages into queue,
 *				   and collect the time events were send.
 *			Solution: Separate runs, Wi-Fi & Mqtt+Button.
 *				   Both send messages to each other via one EventManager tunnel.
 *				   There needs to be a collection of messages within esp32.
 */
void button_event_add_handler(struct button button, esp_event_handler_t handler, void * arg);

#endif //MCU_ESP_BUTTON_H
