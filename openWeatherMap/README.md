# openWeatherMap

## Usage
### Goto mt7688 Terminal by ssh or serial 
* Modify configure file to change mt7688Duo to router mode **vi /etc/config/network**
<pre>
config interface 'lan'
      option proto 'static'
	  option netmask '255.255.255.0'
	  option ipaddr '192.168.100.1'
<br>
config interface 'wan'
	  option ifname 'eth0'
	  option proto 'dhcp'
</pre>
* First you need to enable yunbridge 
<pre>
> uci set yunbridge.config.disabled='0'
> uci commit
</pre>
* Download this repository and move **MT7688_Demo/openWeatherMap** to **/root** path
* Navigate to **openWeatherMap/openWeatherMap_js/script/**
* Setting startup script  
<pre>
> chmod +x openweathermap
> cp openweathermap /etc/init.d/
> /etc/init.d/openweathermap enable
> /etc/init.d/openweathermap start &
> reboot
</pre>


## Using Arduino IDE to upload Arduino sketch
* Download this repository to your computer and copy the Arduino library to your IDE's library path, open openWeatherMap_Arduino.ino with Arduino IDE compile and upload sketch.
