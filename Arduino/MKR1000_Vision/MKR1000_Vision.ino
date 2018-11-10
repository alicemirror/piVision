/**
   @file MKR1000_Vision.ino
   @brief Main application to control HID via WiFi

   This application implement both keyboard and mouse to control
   a computer from remote via WiFi. The prototype application, for
   impaired mobility persons works in conjunction with the
   piVision prototype using a Raspberry PI3 B+ and a Omron Vision
   camera device for movements and gestures detection.

   @section sponsoring Main Sponsors
   Dekimo Experts <dekimo.com>
   Element14 <element14.com>

   @section note Note on the Project
   This prototype aims to explore the possibilities and limits
   offered by the integration of the Omron Vision component creating
   a smart device badrf on the Raspberry PI linux embedded and the
   MKR1000 IoT Arduino family of microprocessors to create OSHW, Open Source
   low cost and easy to use devices to support seriousl impaired mobility
   users.\n
   One of the most important considerations about the usability of this device
   is its non-impacting approach to the host computer: no drivers or special
   applications or software should be installed. The only condition is the
   availability of on-screenkeyboard; this is part of the standard accessibility
   features available on almost all the OS: Windows, Linux, OSX.

   @section colors Color Coding
   The MKR1000 Vision is a simple device to be attacched to a computer USB 2.0+
   port. It is recognized as an external Keyboard/Mouse device. A 8-LEDs NeoPixel
   RGB strip generates constant feedback to the user during the system startup and
   the normal use. \n

   @subsection systart On System Startup
   When the system starts organge wiping scroll means the device is connecting to
   the WiFi. If conection does not succeed, it flashes 5 times purple then stop
   showing a fixed red light.

   After the WiFi connection succeds, the device creates a TCP/IP server accessible on
   the local network through the port 80 and the display flshes 5 times green, the it is
   ready to receive command from the guest. If for some reason it is not possible to
   initialize the server the device flashes 5 times the red light then shows a fixed red
   light and stops.

   When the startup procedure completes the device shows a fixed blue light for 5 seconds
   then it is ready to receive commands.

   @note If the blue light is fixed, the device is in development mode for programming and
   testing. The received comands from the WiFi guest are sent to the serial terminal and
   the device does not interact with the computer host. This is to prevent the difficult to
   test, program and upgrade the device firmware when it runs as a HID device.

   @subsection Operating Conditions

   @version 0.1
   @author Enrico Miglino <enrico.miglino@dekimo.com>
   @author Balearic Dynamics <balearicdynamics@gmail.com>
   @date November 2018
   @license CC 3.0 ND-NC-SA
*/

#include "globals.h"

/** ======================================================
   Application initialization.
   The setup include the AP connection and server setting
   waiting for character sending through TCP/IP protocol
  ====================================================== */
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
      // it is worth to #undef it.
      if (digitalRead(CONTROL_PIN) == LOW) {
        Keyboard.begin();
        Mouse.begin();
        // Display is set to standby condition
        colorWipe(CYAN, STRIP_WIPE_DELAY);
        setColor(CYAN);
        delay(INITIALIZED_DELAY);
        setColor(PIXOFF);
        strip.show(); // Initialize all pixels to 'off'
        // Startup condition
        hidStatus = HID_IDLE;
      } // Production mode
      else {
        // Note that in this case the error condition is used
        // to disable the loop (control pint is enabled so the
        // development mode is set
        errorState = true;
        setColor(PIXWHITE);
      } // Development mode

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
  // by setup() or until the user does not switch the device
  // to maintenance mode
  while ( (!errorState) && (digitalRead(CONTROL_PIN) == LOW)) {
    int j;

    showHIDStatus();    // Update the first pixel status
    // Call the update() function on the MDNS responder
    // every loop iteration to make sure it can detect
    // and respond to the requests.
    mdnsResponder.poll();

    // listen for incoming clients
    WiFiClient client = server.available();
    if (client) {
      hidStatus = HID_IDLEON;
      showHIDStatus();
      if (client.connected()) {
        if (client.available()) {
          hidStatus = HID_RECEIVE;
          showHIDStatus();
          clientCommand = client.read();
          processCommand();
        } // Client available
        else {
          // Client is connected but no new commands are sent,
          // process eventually only the motion commands
          processMotion();
        } // No new commands received
      } // client connected
    } // Available data

    // Check if the development mode has been set via
    // hardware switch. After setting to the maintenance
    // mode the device should be reset as this condition
    // is reset during the setup() only.
    if (digitalRead(CONTROL_PIN)) {
      Keyboard.end();
      Mouse.end();
      errorState = true;
      setColor(PIXWHITE);
#ifdef _DEBUG
      Serial << "Switched to maintenance. Power Off to reset" << endl;
#endif
    } // Enabled development mode
  } // while running condition
} // Loop

// =====================================================
//  External Function
// =====================================================

/**
 * Executes the curret command process only if it
 * is for motion
 */
void processMotion() {
    if( (clientCommand == CURSOR_LEFT) ||
    (clientCommand == CURSOR_RIGHT) ||
    (clientCommand == CURSOR_UP) ||
    (clientCommand == CURSOR_DOWN) ){
        processCommand();
    }
}

/**
   Process the queued command from the client
*/
void processCommand() {

  switch (clientCommand) {
    case CURSOR_LEFT:
      hidStatus = HID_MOTION;
      Mouse.move(0 - MOUSE_STEPS, 0, 0);
      break;
    case CURSOR_RIGHT:
      hidStatus = HID_MOTION;
      Mouse.move(MOUSE_STEPS, 0, 0);
      break;
    case CURSOR_UP:
      hidStatus = HID_MOTION;
      Mouse.move(0, MOUSE_STEPS, 0);
      break;
    case CURSOR_DOWN:
      hidStatus = HID_MOTION;
      Mouse.move(0, 0 - MOUSE_STEPS, 0);
      break;
    case CURSOR_PAUSE:
      hidStatus = HID_IDLEON;
      break;
    case CLICK_LEFT:
      hidStatus = HID_MOTION;
      break;
    case CLICK_MID:
      hidStatus = HID_MOTION;
      break;
    case CLICK_RIGHT:
      hidStatus = HID_MOTION;
      break;
    case PRESS_LEFT:
      hidStatus = HID_PRESSLEFT;
      break;
    case PRESS_MID:
      hidStatus = HID_PRESSMID;
      break;
    case PRESS_RIGHT:
      hidStatus = HID_PRESSRIGHT;
      break;
    case BUTTON_RELEASE:
      hidStatus = HID_IDLEON;
      break;
  } // Comand case switch
  showHIDStatus();
}

/**
   Show the current HID status setting the leftmost pixel
   to the corresponding color
*/
void showHIDStatus() {

  switch (hidStatus) {
    case HID_IDLE:
      setStatusPix(CYAN);
      break;
    case HID_IDLEON:
      setStatusPix(PIXWHITE);
      break;
    case HID_RECEIVE:
      setStatusPix(FIRE5);
      break;
    case HID_DELAY:
      setStatusPix(FIRE1);
      break;
    case HID_MOTION:
      setStatusPix(PIXWHITE);
      break;
    case HID_PRESSLEFT:
      setStatusPix(FIRE6);
      break;
    case HID_PRESSMID:
      setStatusPix(BLUE4);
      break;
    case HID_PRESSRIGHT:
      setStatusPix(PURPLE4);
      break;
  }
}

/**
   Set the (first) status pixel color leaving
   the other pixels intact

   @param c The color
*/
void setStatusPix(uint32_t c) {
  strip.setPixelColor(HID_PIXEL, c);
  strip.show();
  // Slowdown the interaction few ms. Change this value
  // to fine tune the interaction sensitivity
  delay(HID_PAUSE);
}

/**
   Emits a single color flashing sequence

   @param c The color
*/
void flashSignalSequence(uint32_t c) {
  int j;

  for (j = 0; j < CONNECTION_LOOP; j++) {
    colorFlash(c, CONNECTION_LOOP_DELAY);
  }
}

/**
   Call the delayLoop() function with a specific color

   @param c The color
*/
void delayLoop(uint32_t c) {
  delayLoop(c, CONNECTION_DELAY_LOOP);
}

/**
   Executes a delayed On/Off wipe sequence on a single
   color for the desired number of times

   @param c The color
   @param cycle The number of wipe cycles
*/
void delayLoop(uint32_t c, int cycles) {
  int j;

  // Wiping delay loop
  for (j = 0; j < cycles; j++) {
    colorWipeOnOff(c, CONNECTION_DELAY);
  }
}

/**
   Executes a wipe sequence of the desired color

   @param c The color
   @param wait The wipe delay between colors
*/
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

/**
   Executes a bidirectional wipe sequence
   of the desired color

   @param c The color
   @param wait The wipe delay between colors
*/
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

/**
   Flash thedesired color

   @param c The color
   @param wait The flash duration
*/
void colorFlash(uint32_t c, uint8_t wait) {
  setColor(c);
  delay(wait);
  setColor(0);
  delay(wait);
}

/**
   Fill the bar with the desired color

   @param c The color
*/
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

/**
   Print on the serial the full WiFi status

   @warning This function has effect only if
   _DEBUG is defined
*/
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
