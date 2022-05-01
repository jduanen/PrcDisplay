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
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "wifi.h"

#include <PCF8574.h>
#include "sequences.h"

#include <ShiftRegister74HC595.h>
#include "fonts.h"
#include <LedArray.h>


#define APP_VERSION     "1.0.0"

#define VERBOSE             0


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

#define DEF_SEQUENCE_NUMBER 0
#define DEF_SEQUENCE_SPEED  0

#define UNUSED_ANALOG       A0

#define DELAY_INTERVAL      10  // baseDelay is to be multiplied by 10msec
#define RAND_PATT_DELAY     10  // baseDelay for the random pattern case -- 10msec

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

#define STARTUP_MSG         "Hello World!"
#define STARTUP_FONT        SKINNY_FONT


int loopCnt = 0;

bool enableELwires = true;
bool enableLedArray = true;
const int ledPin = 2;

AsyncWebServer  server(WEB_SERVER_PORT);
AsyncWebSocket  ws("/ws");

StaticJsonDocument<200> doc;

PCF8574 prcd = PCF8574(I2C_BASE_ADDR);

uint16_t sequenceNumber = DEF_SEQUENCE_NUMBER;
uint16_t lastSequenceNumber = sequenceNumber;
uint32_t sequenceSpeed = DEF_SEQUENCE_SPEED;
uint32_t lastSequenceSpeed = sequenceSpeed;

LedArray<NUM_SR> leds(DATA_PIN, SRCLK_PIN, RCLK_PIN, OE_PIN, NUM_ROWS, NUM_COLS, STD_WAIT);

String fontNamesList = String();

String ledMessage = STARTUP_MSG;

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
    font-size: 1.9rem;
    color: white;
  }
  h2{
    margin: 5px;
    font-size: 1.7rem;
    font-weight: bold;
    color: #143642;
  }
  h3{
    margin-top: 10px;
    font-size: 1.5rem;
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
  .staticState {
    margin: 5px;
    font-size: 1.2rem;
    color:black;
    font-weight: bold;
  }
  .state {
    font-size: 1.2rem;
    color:#8c8c8c;
    font-weight: bold;
  }
  .switch {
    position: relative; 
    display: inline-block; 
    width: 60px; 
    height: 34px;
    margin-left: 10px;
  } 
  .switch input {
    display: none
  }
  .slider {
    position: absolute; 
    top: 0; left: 0; right: 0; bottom: 0; 
    background-color: #ccc; 
    border-radius: 25px
  }
  .slider:before {
    position: absolute; 
    content: ""; 
    height: 26px; 
    width: 26px; 
    left: 4px; 
    bottom: 4px; 
    background-color: #fff; 
    -webkit-transition: .4s; 
    transition: .4s;
    border-radius: 25px;
  }
  .vertical-center {
    margin-top: 0px;
    margin-bottom: 25px;
    line-height: 2em;
    font-size: 1.2rem;
    color:#8c8c8c;
    font-weight: bold;
  }
  .center {
    font-size: 1.2rem;
    color:#8c8c8c;
    font-weight: bold;
    display: flex;
    justify-content: center;
    align-items: center;
  }
  input:checked+.slider {
    background-color: #b30000;
  }
  input:checked+.slider:before {
    -webkit-transform: translateX(26px);
    -ms-transform: translateX(26px); 
    transform: translateX(26px);
  }
  .green-button {
    background-color: #4CAF50;
    border: 10px;
    color: white;
    padding: 5px 8px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 1.2rem;
  }
  </style>
<title>PrcDisplay Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>PrcDisplay Web Server</h1>
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
      <p class="staticState">Number of EL Wire Sequences: <span style="color:blue" id="numSequences">%NUMBER_OF_SEQUENCES%</span></p>
    </div>
    <br>
    <div class="card">
      <h2>Controls</h2>
      <h3>System</h3>
      <div class="vertical-center" style="line-height: 1.5em;">
        <p>Update Firmware: <a href="update">Update</a></p>
        <p>
          SSID: <input type="text" id="ssid">
          <br>
          Password: <input type="password" id="password">
        </p>
        <p><button class="green-button" id="save" onclick="saveConfiguration()">Save Configuration</button></p>
      </div>

      <h3>LED Array</h3>
      <div class="vertical-center">
        State: &nbsp <span id="ledState" style="color:blue"></span>
        <label class="switch">
          <input type="checkbox" onchange="toggleCheckbox(this)" id="led">
          <span class="slider"></span>
        </label>
      </div>
      <div class="vertical-center">
        Message:
        <input type="text" id="ledMessage">
        <br>
        <input type="button" id="ledMessageSend" value="Send">
        <input type="button" id="ledMessageAppend" value="Append">
        <select id="fonts" name="fonts"></select>
      </div>

      <h3>EL Wires</h3>
      <div class ="center">
        State: &nbsp <span id="elState" style="color:blue"></span>
        <label class="switch">
          <input type="checkbox" onchange="toggleCheckbox(this)" id="el">
            <span class="slider"></span>
        </label>
      </div>
      <div class="vertical-center">
        Sequence Number:
        <select id="sequenceNumber" name="sequenceNumber" onchange="setSequence()"></select>
        <br>
        Sequence Speed:
        <input type="range", id="sequenceSpeed", min="1", max="100", value="50" onchange="setSequence()"><p><span id="speed"></span><p>
      </div>
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
    initView();
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onLoad(event) {
    initWebSocket();
  }
  function initView() {
    var jsonMsg = JSON.stringify({"msgType": "query"});
    websocket.send(jsonMsg);
    document.getElementById('ledMessageSend').addEventListener("click", setLedMsg);
    document.getElementById('ledMessageAppend').addEventListener("click", appendLedMsg);
    var select = document.getElementById('fonts');
    var fontNames = "%FONT_NAMES%";
    var fontNamesArr = fontNames.split(',');
    for (var i = 0; i < fontNamesArr.length; i++) {
      var opt = document.createElement('option');
      opt.value = i;
      opt.innerHTML = fontNamesArr[i];
      select.appendChild(opt);
    }
    var select = document.getElementById('sequenceNumber');
    for (var i = 0; i < %NUMBER_OF_SEQUENCES%; i++) {
      var opt = document.createElement('option');
      opt.value = i;
      opt.innerHTML = i;
      select.appendChild(opt);
    }
  }
  function setLedMsg() {
    var fontNum = document.getElementById('fonts').value;
    var jsonMsg = JSON.stringify({"msgType": "ledMsg", "mode": "set", "fontNum": parseInt(fontNum), "text": document.getElementById('ledMessage').value});
    websocket.send(jsonMsg);
  }
  function appendLedMsg() {
    var fontNum = document.getElementById('fonts').value;
    var jsonMsg = JSON.stringify({"msgType": "ledMsg", "mode": "append", "fontNum": parseInt(fontNum), "text": document.getElementById('ledMessage').value});
    websocket.send(jsonMsg);
  }
  function onMessage(event) {
    var state;
    var elem;
    const msgObj = JSON.parse(event.data);
    console.log('msgObj: ', msgObj);

    elem = document.getElementById('ledState');
    if (msgObj.led == "1"){
      state = "ON";
    } else {
      state = "OFF";
    }
    elem.innerHTML = state;

    elem = document.getElementById('elState');
    if (msgObj.el == "1"){
      state = "ON";
    } else {
      state = "OFF";
    }
    elem.innerHTML = state;

    elem = document.getElementById('ledMessage');
    elem.value = escapeHTML(msgObj.msg);

    elem = document.getElementById('sequenceNumber');
    elem.value = escapeHTML(msgObj.sequenceNumber);

    elem = document.getElementById('sequenceSpeed');
    elem.value = escapeHTML(msgObj.sequenceSpeed);
  }
  function setCheckbox(element, state) {
    document.getElementById(element.id+"State").innerHTML = state;
  }
  function toggleCheckbox(element) {
    var jsonMsg = JSON.stringify({"msgType": element.id, "state": element.checked});
    websocket.send(jsonMsg);
    if (element.checked){
        document.getElementById(element.id+"State").innerHTML = "ON";
    }
    else {
        document.getElementById(element.id+"State").innerHTML = "OFF"; 
    }
  }
  function setSequence() {
    var seqNum = document.getElementById("sequenceNumber").value;
    var seqSpeed = document.getElementById("sequenceSpeed").value;
    console.log("SS: " + seqNum + ", " + seqSpeed);
    var jsonMsg = JSON.stringify({"msgType": "sequence", "sequenceNumber": seqNum, "sequenceSpeed": seqSpeed});
    console.log("setSequence: " + jsonMsg);
    websocket.send(jsonMsg);
    document.getElementById("speed").innerHTML = seqSpeed;
  }
  function saveConfiguration() {
    var ssid = document.getElementById("ssid").value;
    var passwd = document.getElementById("passwd").value;
    console.log("SC: " + ssid + ", " + passwd);
    var jsonMsg = JSON.stringify({"msgType": "saveConf", "ssid": ssid, "passwd": passwd});
    console.log("Save configuration: " + jsonMsg);
    websocket.send(jsonMsg);
  }
  function escapeHTML(s) {
    return s.replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/'/g, '&apos;')
        .replace(/"/g, '&quot;')
        .replace(/\//g, '&sol;');
  }
</script>
</body>
</html>)rawliteral";


void notifyClients() {
  String msg = "{";
  msg += "\"led\": " + String(enableLedArray);
  msg += ", \"el\": " + String(enableELwires);
  msg += ", \"msg\": \"" + ledMessage + "\"";
  msg += ", \"sequenceNumber\": \"" + String(sequenceNumber) + "\"";
  msg += ", \"sequenceSpeed\": \"" + String(sequenceSpeed) + "\"";
  msg += "}";
  ws.textAll(msg);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    DeserializationError error = deserializeJson(doc, (char *)data);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return;
    }
    String msgType = String(doc["msgType"]);
    if (msgType.equals("led")) {
      enableLedArray = !enableLedArray;
    } else if (msgType.equals("el")) {
      enableELwires = !enableELwires;
    } else if (msgType.equals("query")) {
      // NOP
    } else if (msgType.equals("ledMsg")) {
      String mode = String(doc["mode"]);
      ledMessage = String(doc["text"]);
      ledMessage.trim();
      int f = doc["fontNum"];
      if (mode.equals("set")) {
        leds.message(&ledMessage, ('0' + f));
      } else if (mode.equals("append")) {
        leds.appendMessage(&ledMessage, ('0' + f));
      } else {
        Serial.println("Error: unknown mode type: " + mode);
        return;
      }
    }
    notifyClients();
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      printf("WebSocket client #%u disconnected\n", client->id());
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
    return (leds.libVersion);
  } else if (var == "NUMBER_OF_FONTS") {
    return (String(NUM_FONTS));
  } else if (var == "FONT_NAMES") {
    return (fontNamesList);
  } else if (var == "LED_STATE") {
    return (enableLedArray ? "ON" : "OFF");
  } else if (var == "EL_STATE") {
    return (enableELwires ? "ON" : "OFF");
  } else if (var == "NUMBER_OF_SEQUENCES") {
    return (String(NUM_SEQUENCES));
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

  // use floating input as source of randomness
  randomSeed(analogRead(UNUSED_ANALOG));
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

void elWiresRun() {
  int i, randEnbs, delayTime;
  Pattern *seqPtr;

  seqPtr = sequences[sequenceNumber];
  if ((seqPtr->enables == 0) && (seqPtr->baseDelay == 0x00)) {
    randEnbs = random(0, 255);
    delayTime = sequenceSpeed + RAND_PATT_DELAY;
//    Serial.println("Pattern: 0x" + String(randEnbs, HEX) + ", Delay: " + String(delayTime));
    for (i = 0; (i < NUM_EL_WIRES); i++) {
      digitalWrite((i + 2), ((randEnbs >> i) & 0x01));
    }
//    delay(delayTime);  //// FIXME don't do delays, look at loopcount instead
  } else {
    // loop over sequence
    while (!((seqPtr->enables == 0) && (seqPtr->baseDelay == 0x00))) {
      if (sequenceNumber != lastSequenceNumber) {
        lastSequenceNumber = sequenceNumber;
        break;
      }
      if (sequenceSpeed != lastSequenceSpeed) {
        lastSequenceSpeed = sequenceSpeed;
        break;
      }
      delayTime = (seqPtr->baseDelay * DELAY_INTERVAL) + sequenceSpeed;
//      Serial.println("Pattern: 0x" + String(seqPtr->enables, HEX) + ", Delay: " + String(delayTime));
      for (i = 0; (i < NUM_EL_WIRES); i++) {
        digitalWrite((i + 2), ((seqPtr->enables >> i) & 0x01));
      }
//      delay(delayTime);  //// FIXME don't do delays, look at loopcount instead
      seqPtr++;
    }
  }
}


//// FIXME
void initLedArray() {
  println("Fonts Version: " + leds.libVersion);
  println("Number of Fonts: " + String(NUM_FONTS));
  for (int i = 0; (i < NUM_FONTS); i++) {
    if (i > 0) {
      fontNamesList += String(", ");
    }
    fontNamesList += fontNames[i];
  }
  println("Font Names: [" + fontNamesList + "]");

  leds.clear();
  ledMessage = STARTUP_MSG;
  println("Startup message: " + ledMessage);
  leds.message(&ledMessage, STARTUP_FONT);
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
    elWiresRun();
  }

  leds.enableDisplay(enableLedArray);
  if (enableLedArray) {
    leds.run();
  }

  loopCnt++;
};
