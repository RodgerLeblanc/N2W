#pragma once
#include <pebble.h>
#include "clay_settings.h"
#include "commons.h"

void weather_init(void (*f)());
void weather_register_timers();
void weather_cancel_timers();
void weather_timeout();
void weather_retrieved_successfully();
void weather_error_retrieving_weather();
bool weather_is_last_weather_valid(WEATHERDATA *weatherData);
bool weather_is_last_weather_old(WEATHERDATA *weatherData);