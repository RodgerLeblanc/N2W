#pragma once
#include "commons.h"

void outbound_ask_notif2watch_to_refresh();
void outbound_ask_js_to_simulate_notification(int notification_count);
void outbound_ask_js_to_request_weather(int weatherService, char *owm_api_key);