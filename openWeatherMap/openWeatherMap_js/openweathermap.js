/**
 * File: openweathermap.js
 * 
 * Author: lambor
 * Date: 2016-03-09
 *
 * This is a weather forcast recipe using MT7688Duo and Grove sensors,
 * openweathermap's APIs are free to use. You need to register 
 * an account and get your appid, than the APIs can be complete.
 *
 */
 
// To find your city and cityid - http://openweathermap.org/help/city_list.txt
var dest = {
    city: "Wayne", // Remember don't keep space bwtween words
    cityId: '5106292',  
    appid: '29d8cf8c04e193ebdaa88a845d1bf1ae',  // change this id or not, if you have your own id
}

var http = require('http');

var weatherUrls = {
	a_week: {  // api.openweathermap.org/data/2.5/forecast/daily?id={city ID}&appid={appid}
		host: 'api.openweathermap.org',
		path: '/data/2.5/forecast/daily?id=' + dest.cityId + '&appid=' + dest.appid
	},
	current: {  // api.openweathermap.org/data/2.5/weather?q={city name}&appid={appid}		
		host: 'api.openweathermap.org',
		path: '/data/2.5/weather?' + 'q=' + dest.city + '&appid=' + dest.appid
	}
    
};

console.log(weatherUrls.a_week.host + weatherUrls.a_week.path);
console.log(weatherUrls.current.host + weatherUrls.current.path);

function getWeatherForcast(url, callback){
  
  var message = '';
  
	http.get(url, function (res) {
		var body = '';
		res.setEncoding('utf8');
		res.on('data', function (chunk) {
			body += chunk;
		});    
		res.on('end', function(){
			// console.log(body);
			var obj = JSON.parse(body);
			// console.log(obj);
			if(url == weatherUrls.a_week){
        
        message = 'Weather Forcast' + '  ' + 'City: ' + dest.city + '\n'
          + 'Date | Temp | humidity | Weather' + '\n';
          
				obj.list.forEach(function(day){
					var temp = (Number(day.temp.day) - 273.15).toFixed(2);
					var temp_min = (Number(day.temp.min) - 273.15).toFixed(2);
					var temp_max = (Number(day.temp.max) - 273.15).toFixed(2);
					var humidity = Number(day.humidity);
					var date = new Date(Number(day.dt)*1000);	
					message += date.getMonth() + '/' + date.getDate()
						+ '  ' + temp_min + 'C/' + temp_max + 'C'
						+ '  ' + humidity + '%'
						+ '  ' + day.weather[0].description
						+ '\n'
				});
				console.log(message);
				
				callback(message); //, '/tmp/a_week_weather');
			}
			else if(url == weatherUrls.current) {				
        
        message = 'Current Weather' + '  ' + 'City: ' + dest.city + '\n';
          
				var temp = (Number(obj.main.temp) - 273.15).toFixed(2);
				var temp_min = (Number(obj.main.temp_min) - 273.15).toFixed(2);
				var temp_max = (Number(obj.main.temp_max) - 273.15).toFixed(2);
				var humidity = Number(obj.main.humidity);
				var date = new Date(Number(obj.dt)*1000);					
				message += 'Date:' + date.getMonth() + '/' + date.getDate()
					+ ';temp:' + temp_min + '/' + temp_max
					+ ';humidity:' + humidity + '%'
					+ ';weather:' + obj.weather[0].description
					+ '\n';
          
				console.log(message);

				callback(message);  //, '/tmp/current_weather');
			}				
		});
	});
}

function fileWrite(message, path){
	require('fs').writeFile(path, message, function(err){
		if(err != null){
			return console.log('Write file err!');
		}	
		console.log('Saved!');
	});
}

var state = 1;
function main(){
  // console.log("Current weather\n");
  getWeatherForcast(weatherUrls.a_week, function(data){
    fileWrite(data, '/tmp/a_week_weather');
  });
  
  setTimeout(function(){
    // console.log("A week forcast\n");
    getWeatherForcast(weatherUrls.current, function(data){  
      fileWrite(data, '/tmp/current_weather');
    });	
  }, 5000);  
}

main();
