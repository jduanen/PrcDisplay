/***************************************************************************
*
* PrcDisplay application
* 
* Drives eight 9' EL wires using the AFD Driver board and a 7x21 LED display
*  using four 74HC595 shift registers.
*
* Notes:
*  - I2C defaults: SDA=GPIO4 -> D2, SCL=GPIO5 -> D1
*  - connect to http://<ipaddr> for web interface
*  - connect to http://<ipaddr>/update for OTA update of firmware
*
* TODO
*  - Add per-message blink option (using the SRs' OE bits)
*  - Fill out symbols font
*  - Clean up fonts
*  - Add web interface and OTA capabilities
*
****************************************************************************/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "wifi.h"

#include <PCF8574.h>

#include <ShiftRegister74HC595.h>
#include <LedArray.h>


#define APP_VERSION     "1.0.0"

#define VERBOSE             0

int loopCnt = 0;

void print(String str) {
  if (VERBOSE) {
    Serial.print(str);
  }
}

void println(String str) {
  if (VERBOSE) {
    Serial.println(str);
  }
}

#define WEB_SERVER_PORT     80

AsyncWebServer  server(WEB_SERVER_PORT);

#define I2C_BASE_ADDR       0x38    // PCF8574A
//#define I2C_BASE_ADDR       0x20    // PCF8574
#define READ_ADDR           0x4F
#define WRITE_ADDR          0x4E

#define NUM_EL_WIRES        8

//#define SCL_PIN             1
//#define SDA_PIN             2

PCF8574 prcd = PCF8574(I2C_BASE_ADDR);

bool enableELwires = true;

byte lastWires = 0;

void initWires() {
  prcd.pinMode(P0, OUTPUT, HIGH);
  prcd.pinMode(P1, OUTPUT, HIGH);
  prcd.pinMode(P2, OUTPUT, HIGH);
  prcd.pinMode(P3, OUTPUT, HIGH);
  prcd.pinMode(P4, OUTPUT, HIGH);
  prcd.pinMode(P5, OUTPUT, HIGH);
  prcd.pinMode(P6, OUTPUT, HIGH);
  prcd.pinMode(P7, OUTPUT, HIGH);
  prcd.begin();

  for (int wire = 0; (wire < NUM_EL_WIRES); wire++) {
    prcd.digitalWrite(wire, HIGH);
  }
}

void writeAllWires(byte values) { 
  PCF8574::DigitalInput digitalInput;

  digitalInput.p7 = bitRead(values, 7);
  digitalInput.p6 = bitRead(values, 6);
  digitalInput.p5 = bitRead(values, 5);
  digitalInput.p4 = bitRead(values, 4);
  digitalInput.p3 = bitRead(values, 3);
  digitalInput.p2 = bitRead(values, 2);
  digitalInput.p1 = bitRead(values, 1);
  digitalInput.p0 = bitRead(values, 0);

  prcd.digitalWriteAll(digitalInput);
  print("writeAllWires: 0x" +  String(values, HEX) + "; ");
}

#define TEST_NUMBER         1   //// TMP TMP TMP

#define DATA_PIN            14  // D5
#define SRCLK_PIN           12  // D6
#define RCLK_PIN            13  // D7
#define OE_PIN              15  // D8

#define NUM_SR              4
#define NUM_ROWS            7
#define NUM_COLS            21

#define STD_WAIT            35

#define WIDE_FONT           '0'
#define SKINNY_FONT         '1'
#define VERY_SKINNY_FONT    '2'
#define SYMBOLS_FONT        '3'

LedArray<NUM_SR> leds(DATA_PIN, SRCLK_PIN, RCLK_PIN, OE_PIN, NUM_ROWS, NUM_COLS, STD_WAIT);

bool enableLedArray = true;

//// FIXME
void initLedArray() {
  String msg = String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

  println("Fonts Version: " + String(FONTS_VERSION));
  println("Number of Fonts: " + String(NUM_FONTS));
  print("Font Names: [");
  for (int i = 0; (i < NUM_FONTS); i++) {
    print(fontNames[i] + String(", "));
  }
  println("]");

  leds.clear();
  println("Running Test #" + String(TEST_NUMBER));
  switch (TEST_NUMBER) {
    case 0:
      leds.message(&msg, SKINNY_FONT);
    break;
    case 1:
      leds.message("Wide FONT; ", WIDE_FONT);
      leds.appendMessage("Skinny FONT; ", SKINNY_FONT);
      leds.appendMessage("Very Skinny FONT; ", VERY_SKINNY_FONT);
      leds.appendMessage("Symbols FONT: ", WIDE_FONT);
      leds.appendMessage("ABCDEFGHIJKLMNOPQRSTU...", SYMBOLS_FONT);
//      leds.blinkDisplay(9);
    break;
    case 2:
      leds.message(&msg, WIDE_FONT);
    break;
    case 3:
      leds.message(&msg, SKINNY_FONT);
    break;
    case 4:
      leds.message(&msg, VERY_SKINNY_FONT);
    break;
    case 5:
      leds.message(&msg, SYMBOLS_FONT);
    break;
    default:
      println("Error: Invalid Test Number: " + String(TEST_NUMBER));
      break;
  }
}


void setup() { 
  delay(500);
  Serial.begin(19200);
  delay(500);
  Serial.println("\nBEGIN");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.println("Connecting ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    print(".");
  }
  Serial.println("\nConnected to " + String(WLAN_SSID));
  Serial.println("IP address: " + WiFi.localIP().toString());

  //// FIXME
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "PrcDisplay");
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
  println("HTTP server started");

  initWires();

  initLedArray();
}

void loop() {
  //// web interface

  if (enableELwires) {
    byte wireVals = (1 << ((loopCnt >> 10) % 8));
    if (wireVals != lastWires) {
      writeAllWires(wireVals);
      lastWires = wireVals;
      if (VERBOSE) {
        Serial.println("wireVals: 0x" + String(wireVals, HEX));
      }
    }
  }

  if (enableLedArray) {
    leds.run();
  }

  loopCnt++;
};
