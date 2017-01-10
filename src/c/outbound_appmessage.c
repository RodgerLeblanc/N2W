#include <pebble.h>
#include "outbound_appmessage.h"

static bool outbound_outbox_begin(DictionaryIterator **out) {
  	AppMessageResult result = app_message_outbox_begin(out);
  	if(result != APP_MSG_OK) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Can't begin outbox... %i", (int)result);
    	return false;
	}
	return true;
}

static bool outbound_outbox_send() {
  	AppMessageResult result = app_message_outbox_send();
  	if(result != APP_MSG_OK) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Can't send outbox... %i", (int)result);
    	return false;
  	}
	return true;
}

void outbound_ask_notif2watch_to_refresh() {
	// Sends a dumb appMessage to Notif2Watch to force it to refresh
	DictionaryIterator *out;
	if (!outbound_outbox_begin(&out)) return;
	
	int dumbVariable = 111;
	dict_write_int(out, ASK_NOTIF2WATCH_REFRESH, &dumbVariable, sizeof(dumbVariable), true);

	outbound_outbox_send();
}

void outbound_ask_js_to_simulate_notification(int notification_count) {
	DictionaryIterator *out;
	if (!outbound_outbox_begin(&out)) return;

	dict_write_int8(out, NUMBER_OF_SLOTS, notification_count*4);

	outbound_outbox_send();
}

void outbound_ask_js_to_request_weather(int weatherService, char *api_key) {
	DictionaryIterator *out;
	if (!outbound_outbox_begin(&out)) return;

	dict_write_int8(out, WEATHER_REQUEST, weatherService);
	if ((weatherService == (int)WEATHER_OPEN_WEATHER_MAP) || (weatherService == (int)WEATHER_DARK_SKY))
		dict_write_cstring(out, WEATHER_API, api_key);

	outbound_outbox_send();
}