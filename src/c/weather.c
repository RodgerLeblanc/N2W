#include "outbound_appmessage.h"
#include "pebble-helpers.h"
#include "weather.h"

AppTimer *is_weather_shown_timer = NULL;
AppTimer *weather_timer = NULL;

int weather_retries = 0;

void (*fetch_weather_callback)();

void weather_init(void (*f)()) {
	fetch_weather_callback = f;
	weather_register_timers();
}

void weather_register_timers() {
	if (!is_weather_shown_timer)
		is_weather_shown_timer = app_timer_register(10000, fetch_weather_callback, NULL);
	if (!weather_timer)
		weather_timer = app_timer_register(30*60*1000, weather_timeout, NULL);
}

void weather_cancel_timers() {
	app_timer_cancel_safe(weather_timer);
	app_timer_cancel_safe(is_weather_shown_timer);
}

void weather_timeout() {
	weather_timer = NULL;
	fetch_weather_callback();
	weather_timer = app_timer_register(30*60*1000, weather_timeout, NULL);
}

void weather_retrieved_successfully() {
	weather_retries = 0;
	if (is_weather_shown_timer) {
		app_timer_cancel(is_weather_shown_timer);
		is_weather_shown_timer = NULL;
	}
}

void weather_error_retrieving_weather() {
	if (weather_retries < 30) {
		if (is_weather_shown_timer)
			app_timer_reschedule(is_weather_shown_timer, 10000);
		else
			is_weather_shown_timer = app_timer_register(10000, fetch_weather_callback, NULL);
					
		weather_retries++;
	}
}

bool weather_is_last_weather_valid(WEATHERDATA *weatherData) {
	return ((weatherData->icon >= 0) && (weatherData->degree != -1000));
}

bool weather_is_last_weather_old(WEATHERDATA *weatherData) {
	int difference = time(NULL) - weatherData->update_time; // in sec
	return (difference > 30*60);
}
