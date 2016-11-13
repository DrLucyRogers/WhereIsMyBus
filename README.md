#WhereIsMyBus
WhereIsMyBus - alerts when your bus is 3-6 mins away from your bus-stop.
When not in use, it becomes a #Cheerlights light. Used on Project #WhereIsMyBus (link)

Project Uses: 
  - WeMOS D1 with 8 neopixels connected on pin 4 
  - 3D Printed Bus
  - Internet connected Raspberry Pi
  - Cheerlights and TFL API's

This repository contains the Arduino code, Node-RED flow and STL files.
  - Arduino file: BusLocalPiv3.ino
  - Node-RED flow: WhereIsMyBusNodeREDFlow.rtf (Copy the code and import clipboard into Node-RED flow).
  - 3D Printed Bus files:
      - 20161111Wheels2.stl
      - 20161111BusShellj.stl

Dependencies:
  - ESP8266WiFi.h https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
  - PubSubClient.h https://github.com/knolleary/pubsubclient
  - Adafruit_NeoPixel.h https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h

Credits: 
  - Arduino Code - BusLocalPiv3.ino by Andy Stanford-Clark with contributions by James Macfarlane and Lucy Rogers
  - Node-RED flow - Lucy Rogers and Andy Stanford-Clark
  - 3D print bus STL files - Steve Cox for initial design, Lucy Rogers for adaptations.
