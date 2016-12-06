#include <pebble.h>
#include "clay_settings.h"
#include "commons.h"

void clay_migrate_storage_data(CLAYSETTINGS *settings) {
  // Check the last storage scheme version the app used
	int last_storage_version = 0; // default value
	if (persist_exists(STORAGE_VERSION_KEY)) {
		last_storage_version = persist_read_int(STORAGE_VERSION_KEY);
	}
	else { return; }

  if (last_storage_version == STORAGE_VERSION) {
    // No migration necessary
    return;
  }

  // Migrate data
  switch(last_storage_version) {
	case 0: {
		ClaySettingsVersion0 oldSettings;
		persist_read_data(CLAY_SETTINGS_KEY, &oldSettings, sizeof(oldSettings));

		// Delete old data
      	persist_delete(CLAY_SETTINGS_KEY);

		// Migrate to new scheme
		settings->time_background_color = oldSettings.time_background_color;
		settings->notification_background_color = oldSettings.notification_background_color;
		settings->time_text_color = oldSettings.time_text_color;
		
		WeatherData oldWeatherData = oldSettings.last_weather_data;
		WeatherData2 newWeatherData;
		newWeatherData.degree = oldWeatherData.degree;
		newWeatherData.use_fahrenheit = oldWeatherData.use_fahrenheit;
		newWeatherData.icon = oldWeatherData.icon;
		strncpy(newWeatherData.city, oldWeatherData.city, sizeof(oldWeatherData.city));
		newWeatherData.update_time = oldWeatherData.update_time;
		newWeatherData.weatherService = (int)WEATHER_YAHOO; // default value
		settings->last_weather_data = newWeatherData;
		
		settings->vibrate = true; // default value
		settings->battery_level_format = BATTERY_LEVEL_FORMAT_TEXT; // default value
		settings->notification_text_color = GColorBlack; // default value
		
		// Save new data
      	persist_write_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
		break;
	}
	case 1: {
		ClaySettingsVersion1 oldSettings;
		persist_read_data(CLAY_SETTINGS_KEY, &oldSettings, sizeof(oldSettings));

		// Delete old data
      	persist_delete(CLAY_SETTINGS_KEY);

		// Migrate to new scheme
		settings->time_background_color = oldSettings.time_background_color;
		settings->notification_background_color = oldSettings.notification_background_color;
		settings->time_text_color = oldSettings.time_text_color;
		
		WeatherData oldWeatherData = oldSettings.last_weather_data;
		WeatherData2 newWeatherData;
		newWeatherData.degree = oldWeatherData.degree;
		newWeatherData.use_fahrenheit = oldWeatherData.use_fahrenheit;
		newWeatherData.icon = oldWeatherData.icon;
		strncpy(newWeatherData.city, oldWeatherData.city, sizeof(oldWeatherData.city));
		newWeatherData.update_time = oldWeatherData.update_time;
		newWeatherData.weatherService = (int)WEATHER_YAHOO; // default value
		settings->last_weather_data = newWeatherData;
		
		settings->vibrate = oldSettings.vibrate;
		settings->battery_level_format = BATTERY_LEVEL_FORMAT_TEXT; // default value
		settings->notification_text_color = GColorBlack; // default value
		settings->use_animation = true; // default value
		settings->vibrate_on_disconnect = true; // default value
		
		PhoneData phone_data;
		phone_data.battery_percent = 0; // default value
		phone_data.update_time = time(NULL) - (60*60*24*365); // default value
		settings->last_phone_data = phone_data; // default value
		
		// Save new data
      	persist_write_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
		break;
	}
	case 2: {
		ClaySettingsVersion2 oldSettings;
		persist_read_data(CLAY_SETTINGS_KEY, &oldSettings, sizeof(oldSettings));

		// Delete old data
      	persist_delete(CLAY_SETTINGS_KEY);

		// Migrate to new scheme
		settings->time_background_color = oldSettings.time_background_color;
		settings->notification_background_color = oldSettings.notification_background_color;
		settings->time_text_color = oldSettings.time_text_color;
		settings->last_weather_data = oldSettings.last_weather_data;
		settings->vibrate = oldSettings.vibrate;
		settings->battery_level_format = BATTERY_LEVEL_FORMAT_TEXT; // default value
		settings->notification_text_color = GColorBlack; // default value
		settings->use_animation = true; // default value
		settings->vibrate_on_disconnect = true; // default value
		
		PhoneData phone_data;
		phone_data.battery_percent = 0; // default value
		phone_data.update_time = time(NULL) - (60*60*24*365); // default value
		settings->last_phone_data = phone_data; // default value
		
		// Save new data
      	persist_write_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
		break;
	}
	case 3: {
		ClaySettingsVersion3 oldSettings;
		persist_read_data(CLAY_SETTINGS_KEY, &oldSettings, sizeof(oldSettings));
		
		// Delete old data
		persist_delete(CLAY_SETTINGS_KEY);
		
		// Migrate to new scheme
		settings->time_background_color = oldSettings.time_background_color;
		settings->notification_background_color = oldSettings.notification_background_color;
		settings->time_text_color = oldSettings.time_text_color;
		settings->last_weather_data = oldSettings.last_weather_data;
		settings->vibrate = oldSettings.vibrate;
		settings->battery_level_format = oldSettings.battery_level_format;
		
		settings->notification_text_color = GColorBlack; // default value
		settings->use_animation = true; // default value
		settings->vibrate_on_disconnect = true; // default value
		
		PhoneData phone_data;
		phone_data.battery_percent = 0; // default value
		phone_data.update_time = time(NULL) - (60*60*24*365); // default value
		settings->last_phone_data = phone_data; // default value
		
		// Save new data
		persist_write_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
		break;
	}
	case 4: {
		ClaySettingsVersion4 oldSettings;
		persist_read_data(CLAY_SETTINGS_KEY, &oldSettings, sizeof(oldSettings));
		
		// Delete old data
		persist_delete(CLAY_SETTINGS_KEY);
		
		// Migrate to new scheme
		settings->time_background_color = oldSettings.time_background_color;
		settings->notification_background_color = oldSettings.notification_background_color;
		settings->time_text_color = oldSettings.time_text_color;
		settings->notification_text_color = oldSettings.notification_text_color;
		settings->last_weather_data = oldSettings.last_weather_data;
		settings->vibrate = oldSettings.vibrate;
		settings->battery_level_format = oldSettings.battery_level_format;
		
		settings->use_animation = true; // default value
		settings->vibrate_on_disconnect = true; // default value
		
		PhoneData phone_data;
		phone_data.battery_percent = 0; // default value
		phone_data.update_time = time(NULL) - (60*60*24*365); // default value
		settings->last_phone_data = phone_data; // default value
				
		// Save new data
		persist_write_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
		break;
	}
  }

  // Migration is complete, store the current storage scheme version number
  persist_write_int(STORAGE_VERSION_KEY, STORAGE_VERSION);
}
	
bool clay_loadPersistentSettings(CLAYSETTINGS *settings) {
	if (persist_exists(CLAY_SETTINGS_KEY)) {
		int bytesRead = persist_read_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
		APP_LOG(APP_LOG_LEVEL_DEBUG, "loadPersistentSettings() bytesRead: %i/%i", bytesRead, sizeof(*settings));
		return (bytesRead == sizeof(*settings));
	}
	return false;
}

void clay_savePersistentSettings(CLAYSETTINGS *settings, WEATHERDATA *weatherData) {
	settings->last_weather_data = *weatherData;
	persist_write_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
	persist_write_int(STORAGE_VERSION_KEY, STORAGE_VERSION);
}

void clay_set_default_settings(CLAYSETTINGS *settings) {
	if (persist_exists(STORAGE_VERSION_KEY) && persist_exists(CLAY_SETTINGS_KEY)) {
		clay_migrate_storage_data(settings);
		bool ok = clay_loadPersistentSettings(settings);		
		if (ok) { return; }
  	}

	settings->time_background_color = PBL_IF_BW_ELSE(GColorBlack, GColorBlack);
	settings->notification_background_color = PBL_IF_BW_ELSE(GColorWhite, GColorRed);
	settings->time_text_color = PBL_IF_BW_ELSE(GColorWhite, GColorWhite);
	settings->notification_text_color = GColorBlack;
	
	settings->use_animation = true;
	
	WeatherData2 weatherData;
	
	weatherData.degree = -1000;
	weatherData.use_fahrenheit = false;
	weatherData.icon = -1;

	int size = sizeof(settings->last_weather_data.city);
	strncpy(weatherData.city, "", size-1);
    weatherData.city[size-1] = '\0';
	
	weatherData.update_time = time(NULL) - (60*60*24*365);
	weatherData.weatherService = (int)WEATHER_YAHOO;
	settings->last_weather_data = weatherData;
		
	settings->vibrate = true;
	settings->vibrate_on_disconnect = true;
	settings->battery_level_format = 1;
	
	PhoneData phone_data;
	phone_data.battery_percent = 0; // default value
	phone_data.update_time = time(NULL) - (60*60*24*365); // default value
	settings->last_phone_data = phone_data; // default value
	
	persist_write_data(CLAY_SETTINGS_KEY, settings, sizeof(*settings));
}
