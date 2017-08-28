# TheEyeBall
ESP8266-01 based Anamatronic eyeball

The mechanical Design came from below
https://www.thingiverse.com/thing:319978

I used a captive network setup from below so it would be easy to connect to.
https://www.hackster.io/rayburne/esp8266-captive-portal-5798ff?ref=user&ref_id=506&offset=2

The Webpage cam from below which is a really thorough tutorial on getting started with ESP, WiFi, and Websockets
https://tttapa.github.io/ESP8266/Chap14%20-%20WebSocket.html

As well as the below for the html and web scripting
http://www.instructables.com/id/Esp8266Websockets-RGB-LED-controller/

Lastly, so I could host the html on the microprocessor but as a separate standalone file (i.e. not inline with the arduino code), I used the below to check for and read from the files on the internal EEPROM filing system.
http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html
