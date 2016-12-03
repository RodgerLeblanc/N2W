#pragma once

#include <pebble.h>

/* Persistent storage keys */
#define CLAY_SETTINGS_KEY   	(0)
#define STORAGE_VERSION_KEY   	(1)

#define STORAGE_VERSION			(5)

typedef enum BatteryLevelFormat {
  BATTERY_LEVEL_FORMAT_ICON = 0, 
  BATTERY_LEVEL_FORMAT_TEXT = 1,
  BATTERY_LEVEL_FORMAT_CIRCLE = 2,
  BATTERY_LEVEL_DO_NOT_SHOW = 3,
  BATTERY_LEVEL_FORMAT_ICON_PEBBLE_ONLY = 10, 
  BATTERY_LEVEL_FORMAT_TEXT_PHONE_ONLY = 11,
  BATTERY_LEVEL_FORMAT_CIRCLE_PEBBLE_ONLY = 12,
  BATTERY_LEVEL_FORMAT_ICON_PHONE_ONLY = 20,
  BATTERY_LEVEL_FORMAT_TEXT_PEBBLE_ONLY = 21,
  BATTERY_LEVEL_FORMAT_CIRCLE_PHONE_ONLY = 22,
} BatteryLevelFormat;

typedef struct WeatherData {
	int degree;
	bool use_fahrenheit;
	int icon;
	char city[50];
	time_t update_time;
} __attribute__((__packed__)) WeatherData;

typedef struct WeatherData2 {
	int degree;
	bool use_fahrenheit;
	int icon;
	char city[50];
	time_t update_time;
	int weatherService;
} __attribute__((__packed__)) WeatherData2;

typedef struct PhoneData {
	int battery_percent;
	time_t update_time;
} __attribute__((__packed__)) PhoneData;

typedef struct ClaySettingsVersion0 {
	GColor time_background_color;
	GColor notification_background_color;
	GColor time_text_color;
	WeatherData last_weather_data;
} __attribute__((__packed__)) ClaySettingsVersion0;

typedef struct ClaySettingsVersion1 {
	GColor time_background_color;
	GColor notification_background_color;
	GColor time_text_color;
	WeatherData last_weather_data;
	bool vibrate;
} __attribute__((__packed__)) ClaySettingsVersion1;

typedef struct ClaySettingsVersion2 {
	GColor time_background_color;
	GColor notification_background_color;
	GColor time_text_color;
	WeatherData2 last_weather_data;
	bool vibrate;
} __attribute__((__packed__)) ClaySettingsVersion2;

typedef struct ClaySettingsVersion3 {
	GColor time_background_color;
	GColor notification_background_color;
	GColor time_text_color;
	WeatherData2 last_weather_data;
	bool vibrate;
	uint8_t battery_level_format;
} __attribute__((__packed__)) ClaySettingsVersion3;

typedef struct ClaySettingsVersion4 {
	GColor time_background_color;
	GColor notification_background_color;
	GColor time_text_color;
	GColor notification_text_color;
	WeatherData2 last_weather_data;
	bool vibrate;
	uint8_t battery_level_format;
} __attribute__((__packed__)) ClaySettingsVersion4;

typedef struct ClaySettingsVersion5 {
	GColor time_background_color;
	GColor notification_background_color;
	GColor time_text_color;
	GColor notification_text_color;
	bool use_animation;
	WeatherData2 last_weather_data;
	bool vibrate;
	bool vibrate_on_disconnect;
	uint8_t battery_level_format;
	PhoneData last_phone_data;
} __attribute__((__packed__)) ClaySettingsVersion5;

typedef ClaySettingsVersion5 CLAYSETTINGS; // Use CLAYSETTINGS to reference the latest ClaySettingsVersionX
typedef WeatherData2 WEATHERDATA; // Use WEATHERDATA to reference the latest WeatherDataX

void clay_migrate_storage_data(CLAYSETTINGS *settings);
bool clay_loadPersistentSettings(CLAYSETTINGS *settings);
void clay_savePersistentSettings(CLAYSETTINGS *settings, WEATHERDATA *weatherData);
void clay_set_default_settings(CLAYSETTINGS *settings);