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

  if (request.indexOf("01") > 0) {
    digitalWrite(LED_Pin, HIGH);
    rainbowSingle(35);
  } else if (request.indexOf("02") > 0) {
    rainbowFade(35);
  } else if (request.indexOf("03") > 0) {
    nightRider(CRGB(255, 0, 0), CRGB(0, 0, 0), 5, 0);
  } else if (request.indexOf("04") > 0) {
    setAll(CRGB(255, 0, 0));
  } else if (request.indexOf("color") > 0) {
    String color = request.substring(request.indexOf("color"));
    // TODO
  }

  client.flush();

  client.print( header );
  client.print( html_1 );
  client.print( html_2 );
  client.print( html_3 );
  client.print( html_4);

  delay(5);
}

void rainbowSingle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<NUM_LEDS; i++) {
      setLED(Wheel((i+j) & 255), i, false);
    }
    FastLED.show();
    delay(wait);
  }
}

/*
 * Nightrider
 * 
 * c = visor color
 * c2 = background color
 * len = length
 * delayTime = time between updates
 */
void nightRider(CRGB c, CRGB c2, int len, int delayTime) {
  setAll(c2);
  for (int i = 0; i < NUM_LEDS; i++) {  // FORWARD
    setLED(c, i, false);
    if (i-len >= 0) {
      setLED(c2, i-len, false);
    }
    FastLED.show();
    delay(delayTime);
  }
  for (int i = 0; i < NUM_LEDS; i++) {  // BACKWARD
    setLED(c, NUM_LEDS - i, false);
    if ((NUM_LEDS-i) <= NUM_LEDS-len) {
      setLED(NUM_LEDS - i + len -1, c2, false);
      leds[(NUM_LEDS-i) + len - 1] = c2;
    }
    FastLED.show();
    delay(delayTime);
  }
}

void rainbowFade(int delayTime) {
  for(int i = 0; i < 256; i++) {
      setAll(Wheel((i) & 255));
    }
    FastLED.show();
    delay(delayTime);
  }
}

void setHeadlights(bool set) {
  headlightLock = false;
  if (set) {
    setLEDs(headlightColor, headlightStart, headlightEnd, false, 0, true);
    headlightLock = true;
  } else {
    setLEDs(CRGB(0,0,0), headlightStart, headlightEnd, false, 0, true);
    headlightLock = false;
  }
}

void setTaillights(bool set) {
  taillightLock = false;
  if (set) {
    setLEDs(taillightColor, taillightStart, taillightEnd, false, 0, true);
    taillightLock = true;
  } else {
    setLEDs(CRGB(0,0,0), taillightStart, taillightEnd, false, 0, true);
    taillightLock = false;
  }
}

void setLEDs(CRGB c, int startLED, int endLED, bool marquee, int marqueeDelay, bool showNow) {
  for (int i = startLED; i < endLED; i++) {
    setLED(c, i, marquee);
    delay(marqueeDelay);
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
    setLED(c, i, false);
  }
  FastLED.show();
}

CRGB wheel(int WheelPos, int dim) {
  CRGB color;
  if (85 > WheelPos) {
   color.r=0;
   color.g=WheelPos * 3/dim;
   color.b=(255 - WheelPos * 3)/dim;;
  } 
  else if (170 > WheelPos) {
   color.r=WheelPos * 3/dim;
   color.g=(255 - WheelPos * 3)/dim;
   color.b=0;
  }
  else {
   color.r=(255 - WheelPos * 3)/dim;
   color.g=0;
   color.b=WheelPos * 3/dim;
  }
  return color;
}
