#include <Adafruit_NeoPixel.h>
#include "Keyboard.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6           // NeoPixel PIN
#define CONTROL_PIN 7   // Activity Enable PIN
#define NUM_LEDS 8
#define BRIGHTNESS 50

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

void setup() {
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
