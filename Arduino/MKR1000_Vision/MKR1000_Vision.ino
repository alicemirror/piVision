#include "globals.h"

/**
   Application initialization.
   The setup include the AP connection and server setting
   waiting for character sending through TCP/IP protocol
*/
void setup() {
  int retries; ///< retries counter

#ifdef _DEBUG
  Serial.begin(115000);
#endif

  pinMode(CONTROL_PIN, INPUT);

  // Initialize the NeoPixel
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  setColor(FIRE1);
  delay(INIT_DELAY);

  WiFi.config(ip);

  // attempt to connect to Wifi network:
  retries = 0;
  while ( (connectStatus != WL_CONNECTED) && (retries < MAX_CONNECTION_RETRIES)) {
#ifdef _DEBUG
    Serial << "Connection attempt " << retries << endl;
#endif
    // Connect to WPA/WPA2 network.
    // Change this line if using open or WEP network:
    connectStatus = WiFi.begin(NET_SSID, NET_PASSWD);
    // wait before tring connecting again
    delayLoop(FIRE2);
    retries++; // Increment the retry limit counter
  }

  // Check for connecton error
  if (connectStatus != WL_CONNECTED) {
    errorState = true;
    flashSignalSequence(PURPLE3);
    setColor(PIXRED);
#ifdef _DEBUG
    Serial << "Connection error. Stopped." << endl;
#endif
  } // Connection error
  else {
    // Connection is ok, setup will continue
#ifdef _DEBUG
    Serial << "Connected" << endl;
    printWifiStatus();
#endif

    // Start the IP server
    server.begin();

    // Setup the MDNS responder to listen to the configured name.
    // NOTE: You _must_ call this _after_ connecting to the WiFi network and
    // being assigned an IP address.
    if (!mdnsResponder.begin(MDNS_NAME)) {
#ifdef _DEBUG
      Serial << "Failed to start MDNS responder. Stopped" << endl;
#endif
      errorState = true;
      flashSignalSequence(PIXRED);
    setColor(PIXRED);
    } // Responder startup error
    else {
      flashSignalSequence(PIXGREEN);
#ifdef _DEBUG
      Serial << "Server http://" << MDNS_NAME << ".local ready and listening" << endl;
#endif
      // Check if the control pin is disabled else the
      // keyboard emulation is not enabled. The switch
      // state permits to program and debug the application
      // When the application runs in production the debug flag
      // should be undefined
      if (digitalRead(CONTROL_PIN) == false) {
        Keyboard.begin();
      } // Production mode
      else {
        // Note that in this case the error condition is used
        // to disable the loop (control pint is enabled so the
        // development mode is set
        errorState = true;
      } // Development mode
      // Display is set to standby condition
      colorWipe(CYAN, STRIP_WIPE_DELAY);
      setColor(CYAN);
    } // Responder started correctly
  } // WiFi connection is ok
} // Setup

/**
   Main application loop. The loop runs only if the control
   pin is set (low) and the initialization setup() function
   has not generated any error.
*/
void loop() {
  // Loop runs only if no error condition has been generated
  // by setup()
  if (!errorState) {
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
    if (digitalRead(CONTROL_PIN)) {

      //Keyboard.print("Keyboard Emulation Works");
      Keyboard.print('Starting a light loop...!@#0((*)&@adfqropivjio');
      delay(500);

      for (j = 0; j < 22; j++) {
        Keyboard.press(KEY_BACKSPACE);
      }

      Keyboard.press( KEY_RETURN);

      Keyboard.releaseAll();
    }
  } // No error condition
}

// =====================================================
//  NeoPixel Function
// =====================================================

void flashSignalSequence(uint32_t c) {
  int j;

  for (j = 0; j < CONNECTION_LOOP; j++) {
    colorFlash(c, CONNECTION_LOOP_DELAY);
  }
}

void delayLoop(uint32_t c) {
  delayLoop(c, CONNECTION_DELAY_LOOP);
}

void delayLoop(uint32_t c, int cycles) {
  int j;

  // Wiping delay loop
  for (j = 0; j < cycles; j++) {
    colorWipeOnOff(c, CONNECTION_DELAY);
  }
}

//! Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//! Fill the dots one after the other with a color
//! Then off in the same sequence
void colorWipeOnOff(uint32_t c, uint8_t wait) {
  // Pixels on
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
  // Pixels off
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0x00);
    strip.show();
    delay(wait);
  }
}

void colorFlash(uint32_t c, uint8_t wait) {
  setColor(c);
  delay(wait);
  setColor(0);
  delay(wait);
}

void setColor(uint32_t c) {
  uint16_t i;

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
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
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void printWifiStatus() {
#ifdef _DEBUG
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
#endif
}
