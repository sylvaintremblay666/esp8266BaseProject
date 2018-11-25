# esp8266BaseProject

This is a esp8266 base sloeber project with:

- Use of WiFiManager library to setup the WiFi connection via captive portal on a cell phone
- An mDNS responder to reach the server via an hostname (http://_serverName_.local)
- A class to save key/values to the flash memory
- A WebServer with a main page (showing the IP address) and configuration view/edit pages

## Odd issue
When trying to build the mDNS library, I'm getting build errors like:

```/devel/arduino/sloeber-431/arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2/libraries/ESP8266mDNS/ESP8266mDNS.cpp:973:44: note: in expansion of macro 'ARDUINO_BOARD' addServiceTxt("arduino", "tcp", "board", ARDUINO_BOARD);```

To fix this, I edited `arduinoPlugin/packages/esp8266/hardware/esp8266/2.4.2$ view platform.txt` and escaped the `"` around `"ARDUINO_BOARD"` at 3 places.

```-DARDUINO_BOARD=\"{build.board}\"```

instead of

```-DARDUINO_BOARD="{build.board}"``` 
