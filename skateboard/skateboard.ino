#include <ESP8266WiFi.h>
#include <FastLED.h>

const char WiFiPassword[] = "12345678";
const char AP_NameChar[] = "WowGo 2S" ;

bool headlightLock = false;
int headlightStart = 5;
int headlightEnd = 10;
CRGB headlightColor = CRGB(255, 255, 255);

bool taillightLock = false;
int taillightStart = 15;
int taillightEnd = 18;
CRGB taillightColor = CRGB(255, 0, 0);

#define NUM_LEDS 60
#define LED_PIN 0
CRGB leds[NUM_LEDS];

WiFiServer server(80);
 
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
// String html_1 = "<!DOCTYPE html><html><head><title>LED Control</title></head><body><div id='main'><h2>LED Control</h2>";
// String html_2 = "<form id='F1' action='LEDON'><input class='button' type='submit' value='LED ON' ></form><br>";
// String html_3 = "<form id='F2' action='LEDOFF'><input class='button' type='submit' value='LED OFF' ></form><br>";
// String html_4 = "</div></body></html>";
String minifiedHTML = "<!DOCTYPE html><html> <head> <meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'/> <style>.container{padding-right: 15px; padding-left: 15px; margin-right: auto; margin-left: auto;}@media (min-width: 768px){.container{width: 750px;}}@media (min-width: 992px){.container{width: 970px;}}@media (min-width: 1200px){.container{width: 1170px;}}h2{font-size: 2rem;}h2, h3{margin-bottom: 0.5rem; font-family: sans-serif; font-weight: 500; line-height: 1.2;}.btn-primary{color: #fff; background-color: #007bff; border-color: #007bff;}.btn-danger{color: #fff; background-color: #dc3545; border-color: #dc3545;}.btn-success{color: #fff; background-color: #28a745; border-color: #28a745;}.btn{display: inline-block; margin-bottom: 0; font-weight: normal; text-align: center; vertical-align: middle; -ms-touch-action: manipulation; touch-action: manipulation; cursor: pointer; background-image: none; border: 1px solid transparent; white-space: nowrap; padding: 6px 12px; font-size: 14px; line-height: 1.42857143; border-radius: 4px; -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none;}</style> <title>LED Control</title> </head> <body> <div class='container'> <h2>WowGo 2S</h2> <br/> <a href='?h1' ><button type='button' class='btn btn-success'>Headlights On</button></a > <a href='?h0' ><button type='button' class='btn btn-danger'>Headlights Off</button></a > <br/><br/> <a href='?01' ><button type='button' class='btn btn-primary'> Rainbow Single </button></a > <a href='?02' ><button type='button' class='btn btn-primary'>Rainbow Fade</button></a > <a href='?03' ><button type='button' class='btn btn-primary'>Night Rider</button></a > <a href='?04' ><button type='button' class='btn btn-primary'>Red</button></a > <a href='?05' ><button type='button' class='btn btn-primary'>Primary</button></a > <a href='?06' ><button type='button' class='btn btn-primary'>Primary</button></a > <a href='?07' ><button type='button' class='btn btn-primary'>Primary</button></a > <a href='?08' ><button type='button' class='btn btn-primary'>Primary</button></a > <br/> <br/> <form action='' method='get'> <label for='color'>Pick a color</label> <input type='color' id='color' name='color' value='#FFFFFF'/> <button type='submit' class='btn btn-primary'>Go</button> </form> </div></body></html>";
 
String request = "";
int LED_Pin = 0;

int hexToDec(char in);

void setAll(CRGB c);
CRGB wheel(int WheelPos, int dim);
void setLEDs(CRGB c, int startLED, int endLED, bool marquee, int marqueeDelay, bool showNow);
void setLED(CRGB c, int i, bool showNow);
 
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  setAll(CRGB(0,0,0));
 
  boolean conn = WiFi.softAP(AP_NameChar, WiFiPassword);
  server.begin();

  Serial.println("Startup Finished");
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)  {  return;  }

  // Read the first line of the request
  request = client.readStringUntil('\r');

  if (request.indexOf("h1") > 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    setHeadlights(true);
  } else if (request.indexOf("h0") > 0) {
    digitalWrite(LED_BUILTIN, LOW);
    setHeadlights(false);
  } else if (request.indexOf("01") > 0) {
    // rainbowSingle(35);
  } else if (request.indexOf("02") > 0) {
    // rainbowFade(35);
  } else if (request.indexOf("03") > 0) {
    nightRider(CRGB(255, 0, 0), CRGB(0, 0, 0), 5, 0);
  } else if (request.indexOf("04") > 0) {
    setAll(CRGB(255, 0, 0));
  } else if (request.indexOf("color") > 0) {
    // Example request: ?color=%23ff0000
    // +5 for "color" and +4 for "=%23"
    int colorStart = request.indexOf("color") + 5 + 4;
    String color = request.substring(colorStart, colorStart + 6);
    int red = hexToDec(color.charAt(0)) * 16 + hexToDec(color.charAt(1));
    int green = hexToDec(color.charAt(2)) * 16 + hexToDec(color.charAt(3));
    int blue = hexToDec(color.charAt(4)) * 16 + hexToDec(color.charAt(5));
    setAll(CRGB(red, green, blue));
  }

  client.flush();

  client.print( header );
  client.print( minifiedHTML );

  delay(5);
}

int hexToDec(char in) {
  switch (in) {
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'a':
      return 10;
    case 'b':
      return 11;
    case 'c':
      return 12;
    case 'd':
      return 13;
    case 'e':
      return 14;
    case 'f':
      return 15;
  }
  return 0;
}

// void rainbowSingle(uint8_t wait) {
//   uint16_t i, j;
//   for(j=0; j<256; j++) {
//     for(i=0; i<NUM_LEDS; i++) {
//       setLED(wheel((i+j) & 255), i, false);
//     }
//     FastLED.show();
//     delay(wait);
//   }
// }

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

// void rainbowFade(int delayTime) {
//   for(int i = 0; i < 256; i++) {
//       setAll(wheel((i) & 255));
//     }
//     FastLED.show();
//     delay(delayTime);
//   }
// }

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
   color.b=(255 - WheelPos * 3)/dim;
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
