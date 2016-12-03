var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

function iconFromOWMIcon(code)  {
	switch(code) {
		case "01d": return 0; // Clear day
		case "01n": return 1; // Clear night
		case "02d": return 2; // Partly cloudy day
		case "02n": return 3; // Partly cloudy night
		case "03d":
		case "03n":
		case "04d":
		case "04n": return 4; // Cloudy
		case "09d":
		case "09n":
		case "10d":
		case "10n": return 5; // Rain
		case "11d":
		case "11n": return 6; // Thunder
		case "13d":
		case "13n": return 7; // Snow
		case "50d":
		case "50n": return 8; // Fog
		default: return 99; // Not available
	}
} 

function fetchOWMWeather() {
	var settings = {};
	var units = '';
	var city = "";
	var api = "";

	try {
    	settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
		units = settings.TemperatureScale ? 'imperial' : 'metric'; // boolean
		city = settings.WeatherCity;
		api = settings.WeatherApi
	} catch (e) {}

	if (!api) {
		var title = 'Weather error: No API set';
		var body = 'You need to enter an API in the configuration page to get weather.';
		Pebble.showSimpleNotificationOnPebble(title, body);

		return;
	}
	
	if (!city) {
		// Handle error or set a default city
	}
	
	var url = 'http://api.openweathermap.org/data/2.5/weather' +
    	'?q=' + city +
    	'&units=' + units + 
		'&appid=' + api;

	var req = new XMLHttpRequest();
  	req.open('GET', encodeURI(url), true);
  	req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        var response = JSON.parse(req.responseText);
		  if (response) {
			var temperature = Math.round(response.main.temp);
			var icon = iconFromOWMIcon(response.weather[0].icon);

        	console.log(temperature);
        	console.log(icon);
        	Pebble.sendAppMessage({
		  		'WeatherIcon': icon,
          		'WeatherTemp': temperature,
			});
		  }
		  else {
			Pebble.sendAppMessage({
		  		'WeatherError': -1,
			});
		  }
      } else {
        Pebble.sendAppMessage({
		  'WeatherError': -1,
		});
      }
    }
  };
  req.send(null);
}

Pebble.addEventListener('appmessage', function (e) {
  var dict = e.payload;
  if (dict['WeatherRequest'] !== undefined) {
	fetchOWMWeather();
  }
});
