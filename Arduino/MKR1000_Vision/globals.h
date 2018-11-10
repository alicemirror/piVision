/**
 * @file globals.h
 * @brief Global parameters and constants
 */
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiMDNSResponder.h>
#include <Adafruit_NeoPixel.h>
#include "Keyboard.h"
#include "Mouse.h"
#include "Streaming.h"

#ifdef __AVR__
#include <avr/power.h>
#endif

//! #undef _DEBUG to avoid serial log messages.
//! @warning If the keyboard emulation is enabled (CONTROL_PIN true)
//! the serial aoutput should be disabled (#undef _DEBUG)
#define _DEBUG

#define SERVER_PORT 80
//! Set the network SSID parameter depending on
//! your WiFi
#define NET_SSID "Network-Enrico"
//! WiFi WPA Password
#define NET_PASSWD "4nt0n3lla"

//! the MDNS name that the board will respond to
//! Note that the actual MDNS name will have '.local' after
//! the name above.
#define MDNS_NAME "piVisionServer"

#define PIN 6           ///< NeoPixel PIN
#define CONTROL_PIN 7   ///< Activity Enable PIN
#define NUM_LEDS 8      ///< Neopixel leds
#define BRIGHTNESS 50   ///< Default brightness

#define INIT_DELAY 500              ///< Light startup color delay
#define CONNECTION_DELAY_LOOP 20    ///< Number of color wipes while trying to connect
#define CONNECTION_DELAY 25         ///< LED wiping delay per step
#define FLASH_ERROR_LOOP 5          ///< Max tries to initialize the responder before
                                    ///< generating a stable error condition
#define CONNECTION_LOOP 5           ///< Number of flashing sequences
#define CONNECTION_LOOP_DELAY 100   ///< Delay between flashing sequences
#define MAX_CONNECTION_RETRIES 5    ///< Max tries to connect the WiFi before generating
                                    ///< a stable error condition
#define STRIP_WIPE_DELAY 200        ///< Default LED strip wiping delay

//! If the condition error is set to true for some reason
//! during the initialization process, the system is blocked
//! and never start.
boolean errorState = false;

//! Predefined server port
WiFiServer server(80);

//! Create a MDNS responder to listen and respond 
//! to MDNS name requests.
WiFiMDNSResponder mdnsResponder;

//! Network key Index number (needed only for WEP)
//! Not used here
// int keyIndex = 0;             

int connectStatus = WL_IDLE_STATUS;

//! server IP address:
IPAddress ip(192, 168, 0, 5);

// ====================== NeoPixel globals
//! Create the NeoPixels library instance and initialize with the
//! display parameters
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Predefined colors
#define PIXPINK strip.Color(255, 64, 64)
#define PIXWHITE strip.Color(255, 255, 255)
#define PIXRED strip.Color(255, 0, 0)
#define PIXGREEN strip.Color(0, 255, 0)
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
#define CYAN strip.Color(0, 128, 255)
#define PURPLE1 strip.Color(96, 16, 255)
#define PURPLE2 strip.Color(96, 16, 128)
#define PURPLE3 strip.Color(96, 16, 96)
#define PURPLE4 strip.Color(96, 16, 64)
