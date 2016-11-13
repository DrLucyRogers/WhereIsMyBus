/*
WhereIsMyBus on Local Pi

Neopixel bus alerts using using a WeMoS D1 ESP-8266 board and Raspberry Pi

by Andy Stanford-Clark - with embellishments by Lucy Rogers
 May-Nov 2016

 use board "WeMos D1 R2 & mini"
 CPU 160MHz
 4M (3M SPIFFS)
 upload speed 115200
 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

/////////////////////////////////////////////////////////////////////////////////////////////

// Update these with values suitable for your network.
const char* wifi_ssid = "****";
const char* wifi_password = "****";

/////////////////////////////////////////////////////////////////////////////////////////////
//update this with the address of your Pi (probably 192.168.something.something)
#define BROKER "192.168.1.28"
//If you are adding more than one WeMos, each must have a unique CLIENTID. It can be whatever you want.
#define CLIENTID "BusWeMos1"
//change this if the flash time becomes annoying
#define FLASH_TIME 2000 // ms

// subscribe to this for commands:
#define COMMAND_TOPIC "Lights"

//WiFiClientSecure espClient;
WiFiClient espClient;
PubSubClient client(espClient);

//Change this if using different number of neopixels or different pin
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(8, 4); // eight pixels, on pin 4
// pin 4 is D2 on the WeMoS D1 mini

// flashes this colour when connecting to wifi:
static uint32_t wifi_colour = pixel.Color(5, 0, 5); // magenta
// flashes this colour when connecting to MQTT:
static uint32_t mqtt_colour = pixel.Color(0, 5, 5); // cyan

static uint32_t current_LED = 0x000000; // black
static uint32_t LED_state = current_LED;

int next_flash;
boolean flashing = false;

int prev_val = 0;


void setup() {
  
  Serial.begin(9600);


  pixel.begin();
  //pixel.setBrightness(64);
  pixel.show(); // Initialize all pixels to 'off'
  
  setup_wifi();
  client.setServer(BROKER, 1883);
  client.setCallback(callback);
  
}


void setup_wifi() {
  // connecting to wifi
  set_pixels(wifi_colour); 
  
  // We start by connecting to a WiFi network   
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);
  //WiFi.begin(wifi_ssid);

  wait_for_wifi();
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}


void callback(char* topic, byte* payload, unsigned int length) {
  char content[10];
  int count;
  char count_str[3];
  
  // #rrggbbx (where x is number of flashes 1-F, 0 means stay on, * means flash endlessly)
  
  Serial.print("Message arrived: ");

  if (length != 8)
  {
    Serial.print("expected 8 bytes, got ");
    Serial.println(length);
    return;
  }

  // "else"...

  //Serial.println(topic);
  // "+1" to skip over the '#'
  strncpy(content, (char *)(payload+1), 6);
  content[6] = '\0';
  
  Serial.print("payload: '");
  Serial.print(content);
  Serial.println("'");

  uint32_t value = strtol(content, 0, 16);
  //Serial.println(value);


  // get the "x" off the end - one hex digit
  char control = payload[7];
  flashing = false;
  if (control == '*')
  {
    Serial.println("flash");
    flashing = true;
    next_flash = millis() + FLASH_TIME;
    set_pixels(value);
  }
  else
  {
    count_str[0] = control;
    count_str[1] = '\0';
    count = strtol(count_str, 0, 16);
    Serial.println(count);

    if (count==0) {
      // cheerlights
      set_pixels(value);
    }
    else {
      // hopefully this won't ever be longer than the MQTT keepalive time, as we're 
      // not servicing client.loop() while we're doing this
      
      for (int i=0; i<count; i++)
      {
        set_pixels(value);
        delay(FLASH_TIME);
        set_pixels(0);
        delay(FLASH_TIME);
      }
    }
  }

}


void wait_for_wifi()
{
  Serial.println("waiting for Wifi");
  
  // connecting to wifi - magenta
  set_pixels(wifi_colour);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    toggle_pixel();
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}


void reconnect() {

  if (WiFi.status() != WL_CONNECTED) {
      wait_for_wifi();
  }
  
  // Loop until we're reconnected to the broker
  set_pixels(mqtt_colour);
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    
    // Attempt to connect
    if (client.connect(CLIENTID)) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      toggle_pixel();
      //delay(5000);
    }
  }
  
  set_pixels(0); // clear pixel when connected (black)
 
  // subscribe to the command topic
  client.subscribe(COMMAND_TOPIC);
      
}


void loop() {
  int reading;
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // service the flashing if that's what we're doing
  if (flashing && millis() > next_flash)
  {
    toggle_pixel();
    next_flash = millis() + FLASH_TIME;
  }

  
}



void set_pixels(uint32_t colour) {
  _set_pixels(colour);
  // Update current_LED with what the user last requested,
  // so we can toggle it to black and back again.
  current_LED = colour;

}


uint32_t _current_colour;


void _set_pixels(uint32_t colour) {
  for (int i = 0; i < pixel.numPixels(); i++) {
    pixel.setPixelColor(i, colour);
  }

 // Store current actual LED colour
  // (which may be black if toggling code turned it off.)
  _current_colour = colour;
  pixel.show();
}

void toggle_pixel() {

  if (_current_colour == 0) 
  {
    // if it's black, set it to the stored colour
    _set_pixels(current_LED);
  } else {
    // otherwise set it to black
    _set_pixels(0);
  }
}






