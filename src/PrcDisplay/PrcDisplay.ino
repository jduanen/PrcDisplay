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

#define I2C_BASE_ADDR       0x38    // PCF8574A
//#define I2C_BASE_ADDR       0x20    // PCF8574
#define READ_ADDR           0x4F
#define WRITE_ADDR          0x4E

#define NUM_EL_WIRES        8

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

//#define SCL_PIN             1
//#define SDA_PIN             2


bool enableELwires = true;
bool enableLedArray = true;
const int ledPin = 2;

AsyncWebServer  server(WEB_SERVER_PORT);
AsyncWebSocket  ws("/ws");

PCF8574 prcd = PCF8574(I2C_BASE_ADDR);

LedArray<NUM_SR> leds(DATA_PIN, SRCLK_PIN, RCLK_PIN, OE_PIN, NUM_ROWS, NUM_COLS, STD_WAIT);

byte lastWires = 0;

String fontNamesList = String();

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>PrcDisplay Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  h3{
    font-size: 1.3rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .staticState {
     font-size: 1.2rem;
     color:black;
     font-weight: bold;
   }
   .state {
     font-size: 1.2rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>PrcDisplay Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>PrcDisplay WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>System Information</h2>
      <p class="staticState">App Version: <span style="color:blue" id="version">%VERSION%</span></p>
      <p class="staticState">IP Address: <span style="color:blue" id="ipAddr">%IP_ADDR%</span></p>
      <p class="staticState">Connection: <span style="color:blue" id="connection">%CONNECTION%</span></p>
      <p class="staticState">RSSI: <span style="color:blue" id="rssi">%RSSI%</span></p>
      <p class="staticState">LED Fonts Version: <span style="color:blue" id="fontsVersion">%FONTS_VERSION%</span></p>
      <p class="staticState">Number of LED Fonts: <span style="color:blue" id="numFonts">%NUMBER_OF_FONTS%</span></p>
      <p class="staticState">LED Font Names: <span style="color:blue" id="fontsNames">%FONT_NAMES%</span></p>
    </div>
    <br>
    <div class="card">
      <h2>Controls</h2>
      <h3>System</h3>
      <p class="state">Update Firmware: <a href="update">Update</a></p>
      <h3>LED Array</h3>
      <p class="state">LED Array: <span id="ledState">%LED_STATE%</span> 
      <button id="button" class="button">Toggle</button></p>
      <h3>EL Wires</h3>
      <p class="state">EL Wires: <span id="elState">%EL_STATE%</span> 
      <button id="button" class="button">Toggle</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    console.log('event: ', event);
    if (event.data == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('ledState').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>)rawliteral";


void notifyClients() {
  ws.textAll(String(enableLedArray));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      enableLedArray = !enableLedArray;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  if (var == "VERSION") {
    return (APP_VERSION);
  } else if (var == "IP_ADDR") {
    return (WiFi.localIP().toString());
  } else if (var == "CONNECTION") {
    return (WiFi.SSID());
  } else if (var == "RSSI") {
    return (String(WiFi.RSSI()));
  } else if (var == "FONTS_VERSION") {
    return (FONTS_VERSION);
  } else if (var == "NUMBER_OF_FONTS") {
    return (String(NUM_FONTS));
  } else if (var == "FONT_NAMES") {
    return (fontNamesList);
  } else if (var == "LED_STATE") {
    return (enableLedArray ? "ON" : "OFF");
  } else if (var == "EL_STATE") {
    return (enableELwires ? "ON" : "OFF");
  }
  return String();
}

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

//// FIXME
void initLedArray() {
  String msg = String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

  println("Fonts Version: " + String(FONTS_VERSION));
  println("Number of Fonts: " + String(NUM_FONTS));
  for (int i = 0; (i < NUM_FONTS); i++) {
    if (i > 0) {
      fontNamesList += String(", ");
    }
    fontNamesList += fontNames[i];
  }
  println("Font Names: [" + fontNamesList + "]");

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

  //// TMP TMP TMP
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("\nConnected to " + String(WLAN_SSID));
  Serial.println("IP address: " + WiFi.localIP().toString());

  initWebSocket();

  // route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");

  initWires();
  Serial.println("EL wires function started");

  initLedArray();
  Serial.println("LED array function started");
}

void loop() {
  ws.cleanupClients();

  //// TMP TMP TMP
  digitalWrite(ledPin, enableLedArray);

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
