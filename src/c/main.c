#include "pebble.h"
#include "commons.h"
#include "clay_settings.h"
#include "gbitmap_color_palette_manipulator.h"
#include "log.h"
#include "outbound_appmessage.h"
#include "weather_api_key.h"
#include "pebble-helpers.h"
#include "vibration.h"
#include "weather.h"

#include <pebble-events/pebble-events.h>

static Window *window;
static TextLayer *date_layer;
static TextLayer *time_layer;
static Layer *notif_root_layer;
static Layer *notif_layer;
#ifdef PBL_ROUND
static Layer *extra_notif_layer;
#endif
static Layer *weather_layer;
	
static TemplateIcon weather_icon;
TextLayer *weather_value_layer;
char weather_value[16];

static Layer *phone_level_indicator_layer;
static Layer *pebble_level_indicator_layer;

static char date_text[] = "XXX 00";
static char time_text[] = "00:00";

static GFont large_font, font_time, font_date, font_weather, font_battery;

static TemplateIcon template_icons[NUMBER_OF_ICONS];

static PropertyAnimation *prop_anim1 = NULL;
static PropertyAnimation *prop_anim2 = NULL;
static PropertyAnimation *prop_anim3 = NULL;
AppTimer *tap_timer = NULL;
AppTimer *reconnection_delay_timer = NULL;
AppTimer *ask_notif2watch_to_refresh_timer = NULL;

int watchface_used = WATCHFACE_0;
int lastSlotCount = 99;

int debug_simulate_notification_count = -1;
static int phone_percent = 0;
static int pebble_percent = 0;

CLAYSETTINGS settings; // Actual settings object
WEATHERDATA weatherData; // Actual weatherData object

EventHandle battery_state_event_handle;
EventHandle tick_timer_event_handle;
EventHandle app_message_event_handle;
EventHandle connection_event_handle;

static void battery_proc(Layer *layer, GContext *ctx) {
  bool isPhone = (layer == phone_level_indicator_layer);
	
  int stroke_width = 3;
  int inner_offset = 1;
	
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_stroke_color(ctx, settings.notification_text_color);
  graphics_context_set_text_color(ctx, settings.notification_text_color);
  graphics_context_set_fill_color(ctx, settings.notification_text_color);
  graphics_context_set_stroke_width(ctx, stroke_width);
	
  uint8_t charge_percent = isPhone ? phone_percent : pebble_percent;
  
  BatteryLevelFormat format = (BatteryLevelFormat)settings.battery_level_format;
  switch(format) {
  	case BATTERY_LEVEL_FORMAT_ICON: 
  	case BATTERY_LEVEL_FORMAT_ICON_PEBBLE_ONLY: 
  	case BATTERY_LEVEL_FORMAT_ICON_PHONE_ONLY: {
	  if ((format == BATTERY_LEVEL_FORMAT_ICON_PEBBLE_ONLY) && isPhone)
		  return;
	  if ((format == BATTERY_LEVEL_FORMAT_ICON_PHONE_ONLY) && !isPhone)
		  return;
		
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "*** BATTERY_LEVEL_FORMAT_ICON ***");
	  int x_padding = 2;
	  int y_padding = 7;
	  GRect battery_rect = GRect(
		  bounds.origin.x+x_padding+(isPhone?0:4), 
		  bounds.origin.y+y_padding, 
		  bounds.size.w-(2*x_padding)-4,
		  bounds.size.h-(2*y_padding));
	  
	  GRect battery_tip_rect = GRect(
		  bounds.origin.x+x_padding+(isPhone?battery_rect.size.w:0), 
		  bounds.origin.y+y_padding+battery_rect.size.h/3,
		  4,
		  battery_rect.size.h/3);
	  	  
	  // Draw battery
	  graphics_draw_rect(ctx, battery_rect);
	  graphics_draw_rect(ctx, battery_tip_rect);
	  
	  // Fill it
	  GRect fill_available = GRect(
		  battery_rect.origin.x+stroke_width+inner_offset,
		  battery_rect.origin.y+stroke_width+inner_offset,
		  battery_rect.size.w-(2*stroke_width)-(2*inner_offset),
		  battery_rect.size.h-(2*stroke_width)-(2*inner_offset));
	  
      int width = charge_percent * fill_available.size.w / 100;	
	  
	  GRect fill = GRect(
		  fill_available.origin.x+(isPhone?0:fill_available.size.w-width),
	      fill_available.origin.y,
	      width,
	      fill_available.size.h);
	  	  
	  graphics_fill_rect(ctx, fill, 0, GCornerNone);
	  break;
  	}  
  	case BATTERY_LEVEL_FORMAT_TEXT: 
  	case BATTERY_LEVEL_FORMAT_TEXT_PEBBLE_ONLY: 
  	case BATTERY_LEVEL_FORMAT_TEXT_PHONE_ONLY: {
	  if ((format == BATTERY_LEVEL_FORMAT_TEXT_PEBBLE_ONLY) && isPhone)
		  return;
	  if ((format == BATTERY_LEVEL_FORMAT_TEXT_PHONE_ONLY) && !isPhone)
		  return;
		
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "*** BATTERY_LEVEL_FORMAT_TEXT ***");
	  GRect text_rect = GRect(bounds.origin.x+(isPhone?0:1), bounds.origin.y+7, bounds.size.w, bounds.size.h-7);
	  char text[5];
	  snprintf(text, sizeof(text), "%d%%", charge_percent);
	  graphics_draw_text(ctx, text, font_battery, text_rect, GTextOverflowModeWordWrap, (WIDTH>144)?GTextAlignmentCenter:isPhone?GTextAlignmentLeft:GTextAlignmentRight, NULL);
	  break;
  	}
  	case BATTERY_LEVEL_FORMAT_CIRCLE: 
  	case BATTERY_LEVEL_FORMAT_CIRCLE_PEBBLE_ONLY: 
  	case BATTERY_LEVEL_FORMAT_CIRCLE_PHONE_ONLY: {
	  if ((format == BATTERY_LEVEL_FORMAT_CIRCLE_PEBBLE_ONLY) && isPhone)
		  return;
	  if ((format == BATTERY_LEVEL_FORMAT_CIRCLE_PHONE_ONLY) && !isPhone)
		  return;
		
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "*** BATTERY_LEVEL_FORMAT_CIRCLE ***");
	  GRect rect = GRect(bounds.origin.x+(isPhone?3:8), bounds.origin.y+5, 28, 28);
	  GRect text_rect = GRect(bounds.origin.x+(isPhone?-2:3), bounds.origin.y+7, bounds.size.w, bounds.size.h-7);
	  int32_t start = isPhone ? TRIG_MAX_ANGLE * (100 - charge_percent) / 100 : 0;
	  int32_t end = isPhone ? TRIG_MAX_ANGLE : TRIG_MAX_ANGLE * charge_percent / 100;
	  graphics_draw_arc(ctx, rect, GOvalScaleModeFitCircle, start, end);
	
	  char text[4];
	  snprintf(text, sizeof(text), "%d", charge_percent);
	  graphics_draw_text(ctx, text, font_battery, text_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	  break;
  	}
	default: {
	  break;
	}
  }  
}

void set_color() {
	if (window)
    	window_set_background_color(window, settings.time_background_color);
	layer_mark_dirty_safe(notif_root_layer);
	#ifdef PBL_ROUND
	layer_mark_dirty_safe(extra_notif_layer);
	#endif
	if (time_layer)
    	text_layer_set_text_color(time_layer, settings.time_text_color);
	if (date_layer) 
    	text_layer_set_text_color(date_layer, settings.time_text_color);
}

static void removeSpaceAmPm(char *text) {
	for (int i = 0; ((i < (int)sizeof(text)) && (text[i] != '\0')); i++) {
		if (text[i] == ' ') {
			text[i] = '\0';
			return;
		}
	}
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
  if (units_changed & MINUTE_UNIT) {
	clock_copy_time_string(time_text, sizeof(time_text));
	if (!clock_is_24h_style()) {
		removeSpaceAmPm(time_text);
	}
	
	if (debug_simulate_notification_count >= 0)
		text_layer_set_text(time_layer, "10:09");
    else
    	text_layer_set_text(time_layer, time_text);
  }
  if (units_changed & DAY_UNIT) {
	// Percents are currently shown, we don't want to override it yet
	if (tap_timer)
		return;
	  
    // Update the date - Without a leading 0 on the day of the month
    char day_text[4];
    strftime(day_text, sizeof(day_text), "%a", tick_time);
	if (strcmp(day_text, "") != 0) 
		snprintf(date_text, sizeof(date_text), "%s %i", day_text, tick_time->tm_mday);
	else
		snprintf(date_text, sizeof(date_text), "%i/%i", tick_time->tm_mon, tick_time->tm_mday);
	  
	if (debug_simulate_notification_count >= 0)
  		text_layer_set_text(date_layer, "Mon 1");
	else
    	text_layer_set_text(date_layer, date_text);
  }
}

void tap_timeout() {
	tap_timer = NULL;
	
	// Put back normal value to Date text layer
	time_t now = time(NULL);
	handle_tick(localtime(&now), DAY_UNIT);
}

static void debug_simulate_notification_increment() {
	// For debugging only
	pebble_percent = DEBUG_PEBBLE_PERCENT;
	phone_percent = DEBUG_PHONE_PERCENT;
	
	debug_simulate_notification_count++;
	if (debug_simulate_notification_count > 2) {
		debug_simulate_notification_count = 0;
	}
	
	time_t now = time(NULL);
	handle_tick(localtime(&now), MINUTE_UNIT | DAY_UNIT );
	
	outbound_ask_js_to_simulate_notification(debug_simulate_notification_count);
}

static void update_proc(Layer *layer, GContext *ctx) {
	const GRect bounds = layer_get_bounds(layer);
    graphics_context_set_fill_color(ctx, settings.notification_background_color);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
};

static void update_battery_level_layers() {
	layer_mark_dirty_safe(phone_level_indicator_layer);
	layer_mark_dirty_safe(pebble_level_indicator_layer);
}

void battery_handler(BatteryChargeState charge) {
	pebble_percent = charge.charge_percent;
	update_battery_level_layers();
}

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {	
}

static void animate_layer(LayerAnimationData layerAnimationData1, LayerAnimationData layerAnimationData2, LayerAnimationData layerAnimationData3) {
  int duration = settings.use_animation ? 1000 : 0;
	
  prop_anim1 = property_animation_create_layer_frame(layerAnimationData1.layer, &layerAnimationData1.start, &layerAnimationData1.finish);
  Animation *anim1 = property_animation_get_animation(prop_anim1);
  animation_set_duration(anim1, duration);
  animation_set_handlers(anim1, (AnimationHandlers) {
    .stopped = anim_stopped_handler
  }, NULL);
	
  if (!layerAnimationData2.layer) {
	animation_schedule(anim1);
  }
  else {
	prop_anim2 = property_animation_create_layer_frame(layerAnimationData2.layer, &layerAnimationData2.start, &layerAnimationData2.finish);
	Animation *anim2 = property_animation_get_animation(prop_anim2);
	animation_set_duration(anim2, duration);
	  
	prop_anim3 = property_animation_create_layer_frame(layerAnimationData3.layer, &layerAnimationData3.start, &layerAnimationData3.finish);
	Animation *anim3 = property_animation_get_animation(prop_anim3);
	animation_set_duration(anim3, duration);
	  
	Animation *spawn = animation_spawn_create(anim1, anim2, anim3, NULL);
	animation_schedule(spawn);
  }
}

void set_icon_color(GBitmap *icon, GColor color) {
	if (!icon) return;
	if (gbitmap_color_palette_contains_color(color, icon)) return;
	
	gbitmap_fill_all_except(GColorClear, color, true, icon, NULL);
}

void set_weather_bitmap_and_text(int icon, int degree) {
	if (weather_icon.bitmap) {
		gbitmap_destroy(weather_icon.bitmap);
  	}

	weather_icon.bitmap = gbitmap_create_with_resource(ALL_ICONS_34[icon]);
	bitmap_layer_set_compositing_mode(weather_icon.layer, GCompOpSet);
	bitmap_layer_set_bitmap(weather_icon.layer, weather_icon.bitmap);
	set_icon_color(weather_icon.bitmap, settings.notification_text_color);
	
	//snprintf(weather_value, sizeof(weather_value), ((degree < 100) ? "%d°" : "%d"), degree);
	snprintf(weather_value, sizeof(weather_value), "%d", degree);
	text_layer_set_text(weather_value_layer, weather_value);
	text_layer_set_text_color(weather_value_layer, settings.notification_text_color);
	layer_mark_dirty(text_layer_get_layer(weather_value_layer));
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  bool refresh_needed = false;
  bool js_is_ready_signal = false;
	
  APP_LOG(APP_LOG_LEVEL_DEBUG, "*** INBOX ***");
  Tuple *tuple = dict_read_first(iter);
  while (tuple) {
	// Print to console for debug purpose
	switch (tuple->type) {
		case TUPLE_BYTE_ARRAY:
		case TUPLE_CSTRING: {
			log_string_tuple((int)tuple->key, (char*)tuple->value->cstring);
			break;
		}
		case TUPLE_UINT:
		case TUPLE_INT: {
			log_int_tuple((int)tuple->key, tuple->value->uint8);
			break;
		}
	}
  	switch (tuple->key) {
    	case HUB1_ACCOUNT_KEY: 
    	case HUB2_ACCOUNT_KEY: 
		case HUB3_ACCOUNT_KEY: 
    	case HUB4_ACCOUNT_KEY: 
    	case HUB5_ACCOUNT_KEY: 
    	case HUB6_ACCOUNT_KEY: 
    	case HUB7_ACCOUNT_KEY: 
    	case HUB8_ACCOUNT_KEY: {
			int tupleValue = tuple->value->uint8;
			if (template_icons[tuple->key].number != tupleValue) {
				template_icons[tuple->key].number = tupleValue;

				if (template_icons[tuple->key].bitmap) {
					gbitmap_destroy(template_icons[tuple->key].bitmap);
  				}

				template_icons[tuple->key].bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tupleValue]);
				bitmap_layer_set_compositing_mode(template_icons[tuple->key].layer, GCompOpSet);
				bitmap_layer_set_bitmap(template_icons[tuple->key].layer, template_icons[tuple->key].bitmap);
				
				set_icon_color(template_icons[tuple->key].bitmap, settings.notification_text_color);
			}
      		break;
		}
    	case VIBRATE_KEY: {
			settings.vibrate = (tuple->value->uint8 ==1);
      		break;
		}
		case VIBRATE_ON_DISCONNECT_KEY: {
			settings.vibrate_on_disconnect = (tuple->value->uint8 == 1);
      		break;
		}
    	case USE_ANIMATION_KEY: {
			settings.use_animation = (tuple->value->uint8 == 1);
      		break;
		}
    	case PHONE_PERCENT: {
			phone_percent = tuple->value->int16;
			update_battery_level_layers();
      		break;
		}
    	case SLOT_COUNTER_KEY: {
			// Vibrate if new notification
			int count = tuple->value->uint8;
			if ((count > lastSlotCount) && settings.vibrate) { vibration_vibes_1(); }
			lastSlotCount = count;
			
			int this_watchface;
			switch (count) {
				case 0: {
					this_watchface = WATCHFACE_0;
					break;
				}
				case 1:
				case 2:
				case 3: 
				case 4: {
					this_watchface = WATCHFACE_4;
					break;
				}
				case 5:
				case 6: 
				case 7:
				case 8: {
					this_watchface = WATCHFACE_8;
					break;
				}
				default: {
					this_watchface = WATCHFACE_0;
					break;
				}
			}

			if (this_watchface != watchface_used) {
				#ifdef PBL_RECT
				GRect start = layer_get_frame(notif_root_layer);
				#else
				GRect start = layer_get_frame(extra_notif_layer);
				#endif
				GRect finish;
			
				switch (this_watchface) {
					case WATCHFACE_0: {
						#ifdef PBL_RECT
						finish = NOTIF_ROOT_FRAME_0;
						#else
						finish = EXTRA_NOTIF_FRAME_0;
						#endif
						break;
					}
					case WATCHFACE_4: {
						#ifdef PBL_RECT
						finish = NOTIF_ROOT_FRAME_4;
						#else
						finish = EXTRA_NOTIF_FRAME_0;
						#endif
						break;
					}
					case WATCHFACE_8: {
						#ifdef PBL_RECT
						finish = NOTIF_ROOT_FRAME_8;
						#else
						finish = EXTRA_NOTIF_FRAME_8;
						#endif
						break;
					}
				}
				
				/*
				APP_LOG(APP_LOG_LEVEL_DEBUG, "start x:%i y:%i w:%i h:%i", 
						start.origin.x, 
						start.origin.y, 
						start.size.w, 
						start.size.h);
				APP_LOG(APP_LOG_LEVEL_DEBUG, "finish x:%i y:%i w:%i h:%i", 
						finish.origin.x, 
						finish.origin.y, 
						finish.size.w, 
						finish.size.h);
				*/
				
				//APP_LOG(APP_LOG_LEVEL_DEBUG, "this_watchface: %i", (int)this_watchface);
				//APP_LOG(APP_LOG_LEVEL_DEBUG, "grect_equal: %i", grect_equal(&start, &(NOTIF_ROOT_FRAME_0)) ? 1 : 0);
				
				LayerAnimationData layerAnimationDataNull = { 
					.layer = NULL, 
					.start = GRectZero, 
					.finish = GRectZero };

				#ifdef PBL_RECT
				if ((this_watchface == WATCHFACE_0) && (!grect_equal(&start, &(NOTIF_ROOT_FRAME_0)))) {
					LayerAnimationData layerAnimationData1 = { 
						.layer = notif_root_layer,
						.start = start,
						.finish = finish };
					LayerAnimationData layerAnimationData2 = { 
						.layer = text_layer_get_layer(date_layer),
						.start = DATE_FRAME,
						.finish = DATE_FRAME_LOWER };
					LayerAnimationData layerAnimationData3 = { 
						.layer = text_layer_get_layer(time_layer),
						.start = TIME_FRAME,
						.finish = TIME_FRAME_LOWER };
					
					animate_layer(layerAnimationData1, layerAnimationData2, layerAnimationData3);
				}
				else if ((this_watchface != WATCHFACE_0) && (grect_equal(&start, &(NOTIF_ROOT_FRAME_0)))) {
					LayerAnimationData layerAnimationData1 = { 
						.layer = notif_root_layer,
						.start = start,
						.finish = finish };
					LayerAnimationData layerAnimationData2 = { 
						.layer = text_layer_get_layer(date_layer),
						.start = DATE_FRAME_LOWER,
						.finish = DATE_FRAME };
					LayerAnimationData layerAnimationData3 = { 
						.layer = text_layer_get_layer(time_layer),
						.start = TIME_FRAME_LOWER,
						.finish = TIME_FRAME };
					
					animate_layer(layerAnimationData1, layerAnimationData2, layerAnimationData3);
				}
				else {
					LayerAnimationData layerAnimationData1 = { 
						.layer = notif_root_layer,
						.start = start,
						.finish = finish };
					
					animate_layer(layerAnimationData1, layerAnimationDataNull, layerAnimationDataNull);
				}
				#else
				LayerAnimationData layerAnimationData1 = { 
					.layer = extra_notif_layer,
					.start = start,
					.finish = finish };
				
				animate_layer(layerAnimationData1, layerAnimationDataNull, layerAnimationDataNull);
				#endif
				
				watchface_used = this_watchface;
			}
      		break;
		}
    	case COLOR_TIME_BACKGROUND: {
			settings.time_background_color = GColorFromHEX(tuple->value->int32);
			refresh_needed = true;
			break;
		}
    	case COLOR_NOTIFICATION_BACKGROUND: {
			settings.notification_background_color = GColorFromHEX(tuple->value->int32);
			refresh_needed = true;
      		break;
		}
    	case COLOR_TIME_TEXT: {
			settings.time_text_color = GColorFromHEX(tuple->value->int32);
			refresh_needed = true;
      		break;
		}
		case COLOR_NOTIFICATION_TEXT: {
			settings.notification_text_color = GColorFromHEX(tuple->value->int32);
			refresh_needed = true;
			break;
		}
		case TEMPERATURE_SCALE: {
			bool useFahrenheit = (tuple->value->uint8 == 1);
			weatherData.use_fahrenheit = useFahrenheit;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Use fahrenheit: %s", weatherData.use_fahrenheit ? "True" : "False");
			refresh_needed = true;
			break;
		}
		case WEATHER_ICON: {
			if (weather_icon.bitmap) {
				gbitmap_destroy(weather_icon.bitmap);
  			}

			weather_icon.bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tuple->value->int8]);
			bitmap_layer_set_compositing_mode(weather_icon.layer, GCompOpSet);
			bitmap_layer_set_bitmap(weather_icon.layer, weather_icon.bitmap);
			set_icon_color(weather_icon.bitmap, settings.notification_text_color);
			
			weatherData.icon = tuple->value->int8;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather icon: %i", weatherData.icon);
			break;
		}
		case WEATHER_TEMP: {
			if (tuple->value->int16 != -1000) {
				//if (tuple->value->int16<100)
				//	snprintf(weather_value, sizeof(weather_value), "%d°", tuple->value->int16);
				//else
					snprintf(weather_value, sizeof(weather_value), "%d", tuple->value->int16); // No degree symbol
				text_layer_set_text(weather_value_layer, weather_value);
			}
			weatherData.degree = tuple->value->int16;
			weatherData.update_time = time(NULL);

			weather_retrieved_successfully();
			
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Temperature: %d", weatherData.degree);
			break;
		}
		case WEATHER_CITY: {
			if (strcmp(tuple->value->cstring, "CellNinja") == 0) 
				debug_simulate_notification_increment();
			
			if (weatherData.city != tuple->value->cstring) {
				int size = sizeof(weatherData.city);
				strncpy(weatherData.city, tuple->value->cstring, size-1);
      			weatherData.city[size-1] = '\0';
				weatherData.update_time = time(NULL) - (60*60*24); // Force a refresh by setting an old date
				refresh_needed = true;
			}
			break;
		}
		case WEATHER_ERROR: {
			weather_error_retrieving_weather();
		}
		case WEATHER_SERVICE: {
			int weatherService = atoi(tuple->value->cstring);
			if (weatherData.weatherService != weatherService){
				weatherData.weatherService = weatherService;
				weatherData.update_time = time(NULL) - (60*60*24); // Force a refresh by setting an old date
				refresh_needed = true;
			}
			break;
		}
		case BATTERY_LEVEL_FORMAT: {
			settings.battery_level_format = atoi(tuple->value->cstring);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "settings.battery_level_format: %d %d %s", settings.battery_level_format, battery_level_in_text, tuple->value->cstring);
			update_battery_level_layers();
			break;
		}
		case JS_IS_READY: {
			js_is_ready_signal = true;
			weather_timeout();
			break;
		}
		default: {
			break;
		}
  	}
  	tuple = dict_read_next(iter);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "*** END ***");
	
  if (js_is_ready_signal) {
	  outbound_ask_notif2watch_to_refresh();
	  app_timer_register(2000, weather_timeout, NULL);
  } 
	
  if (refresh_needed) {
	window_set_background_color(window, settings.time_background_color);
	layer_mark_dirty_safe(notif_root_layer);
	#ifdef PBL_ROUND
	layer_mark_dirty_safe(extra_notif_layer);
	#endif
	text_layer_set_text_color(time_layer, settings.time_text_color);
	text_layer_set_text_color(date_layer, settings.time_text_color);
	text_layer_set_text_color(weather_value_layer, settings.notification_text_color);
	  
	for (int i = 0; i < NUMBER_OF_ICONS; i++) {
		set_icon_color(template_icons[i].bitmap, settings.notification_text_color);
	}
	set_icon_color(weather_icon.bitmap, settings.notification_text_color); 
	  
	weather_timeout();
	
	app_timer_cancel_safe(ask_notif2watch_to_refresh_timer);
  }
}

void fetch_weather() {
	if (weather_is_last_weather_old(&weatherData) || !weather_is_last_weather_valid(&weatherData)) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Fetch weather request");
		if (weatherData.weatherService == WEATHER_DARK_SKY)
			outbound_ask_js_to_request_weather(weatherData.weatherService, dark_sky_api_key);
		else
			outbound_ask_js_to_request_weather(weatherData.weatherService, owm_api_key);
	}
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
	
  set_color();
	
  window_set_background_color(window, settings.time_background_color);
	
  time_layer = text_layer_create(TIME_FRAME_LOWER);
  text_layer_set_text_color(time_layer, settings.time_text_color);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, font_time);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  date_layer = text_layer_create(DATE_FRAME_LOWER);
  text_layer_set_text_color(date_layer, settings.time_text_color);
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_font(date_layer, font_date);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  #ifdef PBL_ROUND
  extra_notif_layer = layer_create(EXTRA_NOTIF_FRAME_0);
  layer_set_update_proc(extra_notif_layer, update_proc);
  #endif
	
  notif_root_layer = layer_create(NOTIF_ROOT_FRAME_0);
  notif_layer = layer_create(NOTIF_FRAME);
  layer_set_update_proc(notif_root_layer, update_proc);
	
  int widthAllowable = WIDTH/4;
  int offset = ((widthAllowable-34)/2);
  for (int i = 0; i < NUMBER_OF_ICONS; i++) {
	template_icons[i].bitmap = NULL;
	template_icons[i].number = -1;
	#ifdef PBL_RECT
	template_icons[i].position = GRect(widthAllowable*(i%4)+offset, (ICON_SIZE*(i/4))+3, 34, 34);
	template_icons[i].layer = bitmap_layer_create(template_icons[i].position);
	bitmap_layer_set_compositing_mode(template_icons[i].layer, GCompOpSet);
	layer_add_child(notif_layer, bitmap_layer_get_layer(template_icons[i].layer));  
	#else
	int x[8] = { 0, widthAllowable, widthAllowable*2, widthAllowable*3, (WIDTH-ICON_SIZE)/2, ((WIDTH-ICON_SIZE)/2)-ICON_SIZE, ((WIDTH-ICON_SIZE)/2)+ICON_SIZE, WIDTH }; // LAST ONE WILL NOT SHOW
	template_icons[i].position = GRect(x[i]+offset, (i==4)?13:3, 34, 34);
	template_icons[i].layer = bitmap_layer_create(template_icons[i].position);
	bitmap_layer_set_compositing_mode(template_icons[i].layer, GCompOpSet);
	layer_add_child((i<4) ? notif_layer : extra_notif_layer, bitmap_layer_get_layer(template_icons[i].layer));  
	#endif
  }
  #ifdef PBL_ROUND
  layer_add_child(window_layer, extra_notif_layer);
  #endif
  layer_add_child(notif_root_layer, notif_layer);
	
  weather_layer = layer_create(WEATHER_FRAME);
  weather_icon.position = GRect(((WIDTH/4)+3), 1, 34, 34);
  weather_icon.layer = bitmap_layer_create(weather_icon.position);
  bitmap_layer_set_compositing_mode(weather_icon.layer, GCompOpSet);
  layer_add_child(weather_layer, bitmap_layer_get_layer(weather_icon.layer));

  weather_value_layer = text_layer_create(GRect((WIDTH/2), 1, ICON_SIZE, ICON_SIZE-1));
  text_layer_set_text_color(weather_value_layer, GColorBlack);
  text_layer_set_background_color(weather_value_layer, GColorClear);
  text_layer_set_font(weather_value_layer, font_weather);
  text_layer_set_text_alignment(weather_value_layer, GTextAlignmentCenter);
  text_layer_set_text(weather_value_layer, weather_value);
  layer_add_child(weather_layer, text_layer_get_layer(weather_value_layer));
	
  #if defined(PBL_PLATFORM_CHALK) || defined(PBL_PLATFORM_EMERY)
  GFont battery_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  #else
  GFont battery_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  #endif
	
  phone_level_indicator_layer = layer_create(PHONE_LEVEL_INDICATOR_FRAME);
  layer_set_update_proc(phone_level_indicator_layer, battery_proc);
  layer_add_child(weather_layer, phone_level_indicator_layer);

  pebble_level_indicator_layer = layer_create(PEBBLE_LEVEL_INDICATOR_FRAME);
  layer_set_update_proc(pebble_level_indicator_layer, battery_proc);
  layer_add_child(weather_layer, pebble_level_indicator_layer);

  layer_add_child(notif_root_layer, weather_layer);
  layer_add_child(window_layer, notif_root_layer);
  layer_mark_dirty_safe(notif_root_layer);
  #ifdef PBL_ROUND
  layer_mark_dirty_safe(extra_notif_layer);
  #endif
	
  if (weather_is_last_weather_valid(&weatherData)) {
	  set_weather_bitmap_and_text(weatherData.icon, weatherData.degree);
  }
}

static void window_unload(Window *window) {
  text_layer_destroy_safe(time_layer);
  text_layer_destroy_safe(date_layer);

  layer_destroy_safe(phone_level_indicator_layer);
  layer_destroy_safe(pebble_level_indicator_layer);
  text_layer_destroy_safe(weather_value_layer);

  for (int i = 0; i < NUMBER_OF_ICONS; i++) {
	gbitmap_destroy_safe(template_icons[i].bitmap);
	bitmap_layer_destroy_safe(template_icons[i].layer);
  }

  gbitmap_destroy_safe(weather_icon.bitmap);
  bitmap_layer_destroy_safe(weather_icon.layer);

  #ifdef PBL_ROUND
  layer_destroy_safe(extra_notif_layer);
  #endif
  layer_destroy_safe(notif_layer);
  layer_destroy_safe(weather_layer);
  layer_destroy_safe(notif_root_layer);
}

void bluetooth_handler(bool connected) {
	//	This handler is called when BT connection state changes
	if (connected) {
		set_color();
		reconnection_delay_timer = NULL;
		reconnection_delay_timer = app_timer_register(5000, outbound_ask_notif2watch_to_refresh, NULL) ;		
	}
	else {
		// Temporarily change colors without saving them
		window_set_background_color(window, GColorLightGray);
    	text_layer_set_text_color(time_layer, GColorBlack);
    	text_layer_set_text_color(date_layer, GColorBlack);
		
		//text_layer_set_text(phone_level_layer, "");
		if (settings.vibrate_on_disconnect && !quiet_time_is_active())
			vibes_double_pulse();
	}
}

static void init() {
  EventAppMessageHandlers app_message_handlers = {
	  .received = inbox_received_handler
  };
	
  app_message_event_handle = events_app_message_subscribe_handlers(app_message_handlers, NULL);
  events_app_message_request_inbox_size(1024);
  events_app_message_request_outbox_size(1024);
  events_app_message_open();
	
  clay_set_default_settings(&settings);
  weatherData = settings.last_weather_data;
	
  int difference = time(NULL) - settings.last_phone_data.update_time;
  if (difference < 30*60)
	  phone_percent = settings.last_phone_data.battery_percent;
	
  window = window_create();
  window_set_background_color(window, GColorBlack);
  font_battery = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  font_weather = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  font_date = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  font_time = fonts_load_custom_font(resource_get_handle(PBL_IF_RECT_ELSE(RESOURCE_ID_FUTURA_CONDENSED_53, RESOURCE_ID_FUTURA_CONDENSED_43)));
  large_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GOTHIC_34));
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });

  const bool animated = true;
  window_stack_push(window, animated);

  // Update the screen right away
  time_t now = time(NULL);
  handle_tick(localtime(&now), MINUTE_UNIT | DAY_UNIT );
  // And then every minute
  tick_timer_event_handle = events_tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

  bluetooth_handler(bluetooth_connection_service_peek());
  ConnectionHandlers connection_handlers = {
	  .pebble_app_connection_handler = bluetooth_handler
  };
  connection_event_handle = events_connection_service_subscribe(connection_handlers);
	
  battery_state_event_handle = events_battery_state_service_subscribe(&battery_handler);
  battery_handler(battery_state_service_peek());
	
  weather_init(fetch_weather);
}

static void deinit() {
  events_app_message_unsubscribe(app_message_event_handle);
	
  animation_unschedule_all();
  if (prop_anim1) property_animation_destroy(prop_anim1);
  if (prop_anim2) property_animation_destroy(prop_anim2);
  if (prop_anim3) property_animation_destroy(prop_anim3);
	
  weather_cancel_timers();
  app_timer_cancel_safe(tap_timer);
  app_timer_cancel_safe(reconnection_delay_timer);
  app_timer_cancel_safe(ask_notif2watch_to_refresh_timer);
	
  events_battery_state_service_unsubscribe(battery_state_event_handle);
  events_tick_timer_service_unsubscribe(tick_timer_event_handle);
  events_connection_service_unsubscribe(connection_event_handle);
	
  fonts_unload_custom_font(font_time);
  fonts_unload_custom_font(large_font);
	
  PhoneData phone_data;
  phone_data.battery_percent = phone_percent;
  phone_data.update_time = time(NULL);
  settings.last_phone_data = phone_data;
	
  clay_savePersistentSettings(&settings, &weatherData);
	
  window_destroy(window);
}

int main(void) {
  setlocale(LC_ALL, "");
  init();
  app_event_loop();
  deinit();
}
