var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var noCityErrorShown = false;
var weatherApi = "";
var weatherService = -1;
var lastPosition = {};

var settings = {};
var units = '';
var city = "";
var url = "";

function retrieveSettings() {
	try {
    	settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
		units = settings.TEMPERATURE_SCALE ? 'f' : 'c';
		city = settings.WEATHER_CITY;
	} catch (e) {}
}

function unitsToOWMUnits(u) {
	return (u === 'f') ? "imperial" : "metric";
}

function send_dict(slots) {
  var dict = {};
  	dict.JS_IS_READY = "";
	
	if (slots >= 0) {
		dict.HUB1_ACCOUNT_KEY = 46;
		dict.HUB2_ACCOUNT_KEY = 51;
		dict.HUB3_ACCOUNT_KEY = 27;
		dict.HUB4_ACCOUNT_KEY = 28;
		dict.HUB5_ACCOUNT_KEY = 64;
		dict.HUB6_ACCOUNT_KEY = 55;
		dict.HUB7_ACCOUNT_KEY = 23;
		dict.HUB8_ACCOUNT_KEY = 67;
		dict.SLOT_COUNTER_KEY = parseInt(slots);
		dict.PHONE_PERCENT = 68;
		dict.WEATHER_ICON = 0;
		dict.WEATHER_TEMP = 32;
	}
	
  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
}

/*
function appMessageAck(e) {
    console.log("options sent to Pebble successfully");
}

function appMessageNack(e) {
    console.log("options not sent to Pebble: " + e.error.message);
}
*/

var locationOptions = {
	timeout: 10000,
	maximumAge: 0
};

function locationSuccess(pos) {
	lastPosition = pos.coords;
	
	if (weatherService === -1) return;
	
	if (weatherService === 0)
		fetchYahooWeather();
	else
		fetchOWMWeather();
}

function locationError(err) {
	/*
	if (!noCityErrorShown) { 
		var title = 'Weather error: No city set';
		var body = 'You need to enter a city in the configuration page of N2W to get weather.';
		Pebble.showSimpleNotificationOnPebble(title, body);
		noCityErrorShown = true;
	}
	*/

	console.log('ERROR(' + err.code + '): ' + err.message);
}

Pebble.addEventListener("ready", function() {
	retrieveSettings();
	send_dict(-1);
	if (!city)
		navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});

function iconFromYahooIcon(code)  {
	switch(parseInt(code)) {
		case 0: return 19;
		case 1:
		case 2:
		case 3:
		case 4: return 18;
		case 5:
		case 6: return 13;
		case 7: return 17;
		case 8:
		case 9:
		case 10: return 5;
		case 11: 
		case 12: return 12;
		case 13: 
		case 14: 
		case 15: 
		case 16: return 16;
		case 17: return 5;
		case 18: return 15;
		case 19: return 19;
		case 20: 
		case 21: return 6;
		case 22: 
		case 23: 
		case 24: return 19;
		case 25: return 4;
		case 26:
		case 27:
		case 28: return 3;
		case 29: return 10;
		case 30: return 9;
		case 31: return 1;
		case 32: return 0;
		case 33: return 1;
		case 34: return 0;
		case 35: return 13;
		case 36: return 7;
		case 37: 
		case 38: 
		case 39: return 18;
		case 40: return 12;
		case 41: 
		case 42: 
		case 43: return 16;
		case 44: return 9;
		case 45: return 18;
		case 46: return 16;
		case 47: return 18;
		case 3200: return 8;
		default: return 8;
	}
} 

function iconFromOWMIcon(code)  {
	switch(code) {
		case "01d": return 0;
		case "01n": return 1;
		case "02d": return 9;
		case "02n": return 10;
		case "03d":
		case "03n":
		case "04d":
		case "04n": return 3;
		case "09d":
		case "09n":
		case "10d":
		case "10n": return 12;
		case "11d":
		case "11n": return 18;
		case "13d":
		case "13n": return 16;
		case "50d":
		case "50n": return 6;
		default: return 8;
	}
} 

function fetchYahooWeather() {
	console.log('fetchYahooWeather()');

	if (city) {
		if (city === "CellNinja")
			city = "San Francisco, USA";
	
		url = 'https://query.yahooapis.com/v1/public/yql?q=select item.condition ' +
    		'from weather.forecast where woeid in ' +
    		'(select woeid from geo.places(1) where ' +
    		'text="' + city + '") and ' +
    		'u=\'' + units + '\'&format=json';
	}
	else {
		if (lastPosition.latitude !== undefined) {
			url = 'https://query.yahooapis.com/v1/public/yql?q=select item.condition ' +
    			'from weather.forecast where woeid in ' +
    			'(select woeid from geo.places(1) where ' +
    			'text="(' + lastPosition.latitude + ',' + lastPosition.longitude + ')") and ' +
    			'u=\'' + units + '\'&format=json';
		}
		else {
			return;
		}
	}	

	var req = new XMLHttpRequest();
	//console.log(encodeURI(url));
  	req.open('GET', encodeURI(url), true);
  	req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
        var response = JSON.parse(req.responseText);
		  if (response.query.count > 0) {
			var temperature = Math.round(response.query.results.channel.item.condition.temp);
			var icon = iconFromYahooIcon(response.query.results.channel.item.condition.code);

        	console.log(temperature);
        	console.log(icon);
        	Pebble.sendAppMessage({
		  		'WEATHER_ICON': icon,
          		'WEATHER_TEMP': temperature,
			});
		  }
		  else {
			Pebble.sendAppMessage({
		  		'WEATHER_ERROR': -1,
			});
		  }
      } else {
        console.log('Error');
        Pebble.sendAppMessage({
		  'WEATHER_ERROR': -1,
		});
      }
    }
  };
  req.send(null);
}

function fetchOWMWeather() {
	console.log('fetchOWMWeather()');

	if (city) {
		if (city === "CellNinja")
			city = "San Francisco, USA";
	
		url = 'http://api.openweathermap.org/data/2.5/weather' +
    		'?q=' + city +
    		'&units=' + unitsToOWMUnits(units) + 
			'&appid=' + weatherApi;
	}
	else {
		if (lastPosition.latitude !== undefined) {
			url = 'http://api.openweathermap.org/data/2.5/weather' +
    			'?lat=' + lastPosition.latitude +
    			'&lon=' + lastPosition.longitude + 
    			'&units=' + unitsToOWMUnits(units) + 
				'&appid=' + weatherApi;
		}
		else {
			return;
		}
	}
	
	var req = new XMLHttpRequest();
	//console.log(encodeURI(url));
  	req.open('GET', encodeURI(url), true);
  	req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        //console.log(req.responseText);
        var response = JSON.parse(req.responseText);
		  if (response) {
			//console.log(JSON.stringify(response))
			var temperature = Math.round(response.main.temp);
			var icon = iconFromOWMIcon(response.weather[0].icon);

        	console.log(temperature);
        	console.log(icon);
        	Pebble.sendAppMessage({
		  		'WEATHER_ICON': icon,
          		'WEATHER_TEMP': temperature,
			});
		  }
		  else {
			Pebble.sendAppMessage({
		  		'WEATHER_ERROR': -1,
			});
		  }
      } else {
        console.log('Error');
        Pebble.sendAppMessage({
		  'WEATHER_ERROR': -1,
		});
      }
    }
  };
  req.send(null);
}

Pebble.addEventListener('appmessage', function (e) {
  console.log('message! ' + JSON.stringify(e));
  var dict = e.payload;
  if (dict.NUMBER_OF_SLOTS !== undefined) {
	  send_dict(dict.NUMBER_OF_SLOTS);
  }
  if (dict.WEATHER_API) {
	  weatherApi = dict.WEATHER_API;
  }
  if (dict.WEATHER_REQUEST !== undefined) {
	  weatherService = dict.WEATHER_REQUEST;
	  console.log('weatherService: ' + weatherService);
	  
	  retrieveSettings();
	  
	  if (!city) { 
		  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions); 
		  return;
	  }

	  if (weatherService === 0)
		fetchYahooWeather();
	  else
		fetchOWMWeather();
  }
});
