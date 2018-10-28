#include <ESP8266WiFi.h>
#include <FastLED.h>

const char WiFiPassword[] = "12345678";
const char AP_NameChar[] = "LEDControl" ;

bool headlightLock = false;
int headlightStart = 5;
int headlightEnd = 10;
CRGB headlightColor = CRGB(255, 255, 255);

bool taillightLock = false;
int taillightStart = 15;
int taillightEnd = 18;
CRGB taillightColor = CRGB(255, 0, 0);

const int NUM_LEDS = 60;
const int LED_PIN = 6;
const int LED_BRIGHTNESS = 255;
CRGB leds[NUM_LEDS];

WiFiServer server(80);
 
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
String html_1 = "<!DOCTYPE html><html><head><title>LED Control</title></head><body><div id='main'><h2>LED Control</h2>";
String html_2 = "<form id='F1' action='LEDON'><input class='button' type='submit' value='LED ON' ></form><br>";
String html_3 = "<form id='F2' action='LEDOFF'><input class='button' type='submit' value='LED OFF' ></form><br>";
String html_4 = "</div></body></html>";
 
String request = "";
int LED_Pin = D4;
 
void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
  setAll(CRGB(0,0,0));
  pinMode(LED_Pin, OUTPUT);
 
  boolean conn = WiFi.softAP(AP_NameChar, WiFiPassword);
  server.begin();
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)  {  return;  }

  // Read the first line of the request
  request = client.readStringUntil('\r');

  if       ( request.indexOf("LEDON") > 0 )  { digitalWrite(LED_Pin, HIGH);  }
  else if  ( request.indexOf("LEDOFF") > 0 ) { digitalWrite(LED_Pin, LOW);   }

  client.flush();

  client.print( header );
  client.print( html_1 );
  client.print( html_2 );
  client.print( html_3 );
  client.print( html_4);

  delay(5);
}

void setHeadlights(bool set) {
  headlightLock = false;
  if (set) {
    setLEDs(headlightColor, headlightStart, headlightEnd);
  } else {
    setLEDs(CRGB(0,0,0), headlightStart, headlightEnd);
  }
  headlightLock = true;
}

void setTaillights(bool set) {
  taillightLock = false;
  if (set) {
    setLEDs(taillightColor, taillightStart, taillightEnd);
  } else {

  }
  taillightLock = true;
}

void setLEDs(CRGB c, int startLED, int endLED, bool showNow) {
  for (int i = startLED; i < endLED; i++) {
    setLED(
  }
  if (showNow) {
    FastLED.show();
  }
}

void setLED(CRGB c, int i, bool showNow) {
  if (headlightLock) {
    if (i > headlightStart && i < headlightEnd) {
      return;
    }
  }
  if (taillightLock) {
    if (i > taillightStart && i < taillightEnd) {
      return;
    }
  }
  leds[i] = c;
  if (showNow) {
    FastLED.show();
  }
}

void setAll(CRGB c) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = c;
  }
  FastLED.show();
}
