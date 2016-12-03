module.exports = 
[
  {
    "type": "heading",
    "defaultValue": "N2W"
  },
  {
    "type": "text",
    "defaultValue": "Setup the app here."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Color"
      },
      {
        "type": "color",
        "messageKey": "COLOR_TIME_BACKGROUND",
        "defaultValue": "0x000000",
        "label": "Date & Time Background Color",
        "sunlight": true
      },
      {
        "type": "color",
        "messageKey": "COLOR_TIME_TEXT",
        "defaultValue": "0xFFFFFF",
        "label": "Date & Time Text Color",
        "sunlight": true
      },
	  {
		"type": "color",
		"messageKey": "COLOR_NOTIFICATION_BACKGROUND",
		"defaultValue": "0xFF0000",
		"label": "Notification Background Color",
		"sunlight": true
	  },
	  {
		"type": "color",
		"messageKey": "COLOR_NOTIFICATION_TEXT",
		"defaultValue": "0x000000",
		"label": "Notification Text Color",
		"description": "This option will also color icons, which takes more battery. Leave to black to save battery.",
		"sunlight": true
	  }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Animation"
      },
      {
        "type": "toggle",
        "defaultValue": true,
        "messageKey": "USE_ANIMATION_KEY",
        "label": "Use animation"
      },
	  {
		"type": "text",
    	"defaultValue": "As notifications are usually frequent on Android, animations are running frequently on N2W. To reduce battery impact, turn animations off."
  	  }
	]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Weather"
      },
      {
        "type": "input",
        "messageKey": "WEATHER_CITY",
		"label": "City  (ie: London, UK)",
		"description": "If you don't enter a city, your GPS will be used to retrieve your position"
      },
      {
        "type": "toggle",
        "defaultValue": false,
        "messageKey": "TEMPERATURE_SCALE",
        "label": "Using Fahrenheit"
      },
	  {
		"type": "select",
		"messageKey": "WEATHER_SERVICE",
		"defaultValue": "0",
		"label": "Weather Service",
		"options": [
    		{ 
      			"label": "Yahoo",
      			"value": "0" 
    		},
    		{ 
    			"label": "OpenWeatherMap",
    			"value": "1" 
    		}
  		]
	  }
	]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Vibration"
      },
      {
        "type": "toggle",
        "defaultValue": true,
        "messageKey": "VIBRATE_KEY",
        "label": "Vibrate on new notification"
      },
	  {
        "type": "toggle",
        "defaultValue": true,
        "messageKey": "VIBRATE_ON_DISCONNECT_KEY",
        "label": "Vibrate on bluetooth disconnection"
      }
	]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Battery Indicator"
      },
	  {
		"type": "select",
		"messageKey": "BATTERY_LEVEL_FORMAT",
		"defaultValue": "1",
		"label": "Battery level shown as",
		"options": [
    		{ 
      			"label": "Icon",
      			"value": "0" 
    		},
    		{ 
      			"label": "Icon - Pebble only",
      			"value": "10" 
    		},
    		{ 
      			"label": "Icon - Phone only",
      			"value": "20" 
    		},
    		{ 
    			"label": "Text",
    			"value": "1" 
    		},
    		{ 
      			"label": "Text - Pebble only",
      			"value": "11" 
    		},
    		{ 
      			"label": "Text - Phone only",
      			"value": "21" 
    		},
			{
				"label": "Circle",
				"value": "2"
			},
    		{ 
      			"label": "Circle - Pebble only",
      			"value": "12" 
    		},
    		{ 
      			"label": "Circle - Phone only",
      			"value": "22" 
    		},
			{
				"label": "Do not show",
				"value": "3"
			},
  		]
	  }
	]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
]
;