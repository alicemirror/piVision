#include <SPI.h>
#include <WiFi101.h>
#include <WiFiMDNSResponder.h>
#include <Adafruit_NeoPixel.h>
#include "Keyboard.h"
#include "Streaming.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define _DEBUG

// the IP address:
IPAddress ip(192, 168, 57, 120);

//char ssid[] = "piVIsion";     // your network SSID (name) 
//char pass[] = "piVision";     // your network password    
char ssid[] = "QSpinVisitor";     // your network SSID (name) 
char pass[] = "QSpinGuest2015!";     // your network password    
int keyIndex = 0;             // your network key Index number (needed only for WEP) 

char mdnsName[] = "wifi101"; // the MDNS name that the board will respond to
// Note that the actual MDNS name will have '.local' after
// the name above, so "wifi101" will be accessible on
// the MDNS name "wifi101.local".
String getContent="";
bool isGet=false;
String gotContent="";
char* buf="";
int status = WL_IDLE_STATUS;

#define PIN 6           // NeoPixel PIN
#define CONTROL_PIN 7   // Activity Enable PIN
#define NUM_LEDS 8
#define BRIGHTNESS 50

#define CONNECTION_DELAY 5000 // Seconds to wait after WiFi connection comletes

// Create a MDNS responder to listen and respond to MDNS name requests.
WiFiMDNSResponder mdnsResponder;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

// Predefined colors
#define PINK strip.Color(255, 64, 64)
#define WHITE strip.Color(255, 255, 255)

#define FIRE1 strip.Color(255, 64, 0)
#define FIRE2 strip.Color(255, 96, 0)
#define FIRE3 strip.Color(255, 128, 0)
#define FIRE4 strip.Color(255, 96, 32)
#define FIRE5 strip.Color(255, 32, 64)
#define FIRE6 strip.Color(255, 32, 96)

#define BLUE1 strip.Color(0, 0, 255)
#define BLUE2 strip.Color(0, 32, 255)
#define BLUE3 strip.Color(0, 64, 255)
#define BLUE4 strip.Color(0, 96, 255)

#define PURPLE1 strip.Color(96, 16, 255)
#define PURPLE2 strip.Color(96, 16, 128)
#define PURPLE3 strip.Color(96, 16, 96)
#define PURPLE4 strip.Color(96, 16, 64)

WiFiServer server(80);

void setup() {

  #ifdef _DEBUG
    Serial.begin(115000);
  #endif
  
  WiFi.config(ip);

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
     #ifdef _DEBUG
       Serial << "Attempting to connect to SSID: "<< ssid << endl;
     #endif
     // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
     status = WiFi.begin(ssid, pass);
     // wait 10 seconds for connection:
     delay(CONNECTION_DELAY);
  }
  #ifdef _DEBUG
    Serial << "Connected to " << ssid << endl;
  #endif

  #ifdef _DEBUG
    printWifiStatus();
  #endif
  server.begin();

  // Setup the MDNS responder to listen to the configured name.
  // NOTE: You _must_ call this _after_ connecting to the WiFi network and
  // being assigned an IP address.
  if (!mdnsResponder.begin(mdnsName)) {
    Serial.println("Failed to start MDNS responder!");
    while(1);
  }
  Serial.print("Server listening at http://");
  Serial.print(mdnsName);
  Serial.println(".local/");  
  //--------------------------------------------------------------------------------  
  
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  pinMode(CONTROL_PIN, INPUT);
  
  // End of trinket special code
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

    if(CONTROL_PIN) {
        Keyboard.begin();
    }
  
}

void loop() {
    int j;

  // Call the update() function on the MDNS responder every loop iteration to
  // make sure it can detect and respond to name requests.
  mdnsResponder.poll();

  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);        
      }
    }
  }
  if(digitalRead(CONTROL_PIN)) {

    //Keyboard.print("Keyboard Emulation Works");
    Keyboard.print('Starting a light loop...!@#0((*)&@adfqropivjio');
    delay(500);

    for(j = 0; j < 22; j++) {
        Keyboard.press(KEY_BACKSPACE);
    }

    Keyboard.press( KEY_RETURN);

    Keyboard.releaseAll();    
    
    // ==========================================
    // TEST FUNCTIONS
    // ==========================================
    
    // Rotate the strip with one color on and off 
    colorWipeOnOff(FIRE1, 10);
    colorWipeOnOff(FIRE2, 10);
    colorWipeOnOff(FIRE3, 10);
    
    // Rotate the strip with one color
    colorWipe(FIRE1, 10);
    colorWipe(FIRE2, 10);
    colorWipe(FIRE3, 10);
    colorWipe(FIRE4, 10);
    colorWipe(FIRE5, 10);
    colorWipe(FIRE6, 10);
    
    // Flash colors
    colorFlash(FIRE1, 10);
    colorFlash(FIRE2, 10);
    colorFlash(FIRE3, 10);
    colorFlash(FIRE4, 10);
    colorFlash(FIRE5, 10);
    colorFlash(FIRE6, 10);
    
    // Fixed colors
    setColor(FIRE1);
    delay(1000);
    setColor(FIRE2);
    delay(1000);
    setColor(FIRE3);
    delay(1000);
    setColor(FIRE4);
    delay(1000);
    setColor(FIRE5);
    delay(1000);
    setColor(FIRE6);
    delay(1000);
    
    setColor(BLUE1);
    delay(500);
    setColor(BLUE2);
    delay(500);
    setColor(BLUE3);
    delay(500);
    setColor(BLUE4);
    delay(500);
    
    setColor(PURPLE1);
    delay(500);
    setColor(PURPLE2);
    delay(500);
    setColor(PURPLE1);
    delay(500);
    setColor(PURPLE4);
    delay(500);

    rainbow(10);
    rainbowCycle(10);
    }
}

// =====================================================
//  NeoPixel Function
// =====================================================

//! Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//! Fill the dots one after the other with a color
//! Then off in the same sequence
void colorWipeOnOff(uint32_t c, uint8_t wait) {
  // Pixels on
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
  // Pixels off
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0x00);
    strip.show();
    delay(wait);
  }
}

void colorFlash(uint32_t c, uint8_t wait) {
    setColor(c);
    delay(wait);
    setColor(0);
}

void setColor(uint32_t c) {
    uint16_t i;
    
    for(i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
    }
    strip.show();
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

#ifdef _DEBUG
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
#endif
