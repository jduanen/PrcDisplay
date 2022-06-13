/***************************************************************************
*
* PrcDisplay application
* 
* Drives eight 9' EL wires using the AFD Driver board and a 7x21 LED display
*  using four 74HC595 shift registers.
* 
* N.B. This is insecure in that it passes the WiFi password in the clear -- you've been warned...
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
*
****************************************************************************/

#include <Arduino.h>
#include <ArduinoJson.h>
#define CS_USE_LITTLEFS     true
#include <ConfigStorage.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>

#include "wifi.h"
#include "sequences.h"
#include "ElWires.h"
#include "fonts.h"
#include "LedArray.h"


#define APP_VERSION         "1.0.0"

#define VERBOSE             1

#define WEB_SERVER_PORT     80

#define MAX_WIFI_RETRIES    8 //64

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

#define STARTUP_EL_SEQUENCE 0
#define STARTUP_EL_SPEED    100

#define CONFIG_PATH         "/config.json"


//// TODO add this to a common library
String rot47(String str) {
  String outStr = "";
  char oldChr, newChr;

  for (int i = 0; (i < str.length()); i++) {
    oldChr = str.charAt(i);
    if ((oldChr >= '!') && (oldChr <= 'O')) {
      newChr = ((oldChr + 47) % 127);
    } else {
      if ((oldChr >= 'P') && (oldChr <= '~')) {
        newChr = ((oldChr - 47) % 127);
      } else {
        newChr = oldChr;
      }
    }
    outStr.concat(newChr);
  }
  return(outStr);
}


typedef struct {
  String ssid;
  String passwd;
  bool ledState;
  String ledMessage;
  char ledFont;
  bool elState;
  bool randomSequence;
  unsigned short sequenceNumber;
  unsigned short sequenceSpeed;
} ConfigState;

ConfigState configState = {
  String(WLAN_SSID),
  String(rot47(WLAN_PASS)),
  true,
  String(STARTUP_MSG),
  STARTUP_FONT,
  true,
  false,
  STARTUP_EL_SEQUENCE,
  STARTUP_EL_SPEED
};

ConfigStorage cs(CONFIG_PATH);

AsyncElegantOtaClass AsyncElegantOTA;

const int ledPin = 2;

AsyncWebServer  server(WEB_SERVER_PORT);
AsyncWebSocket  ws("/ws");

StaticJsonDocument<256> wsMsg;
StaticJsonDocument<512> config;

ElWires elWires;

LedArray<NUM_SR> leds(DATA_PIN, SRCLK_PIN, RCLK_PIN, OE_PIN, NUM_ROWS, NUM_COLS, STD_WAIT);

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
  .green-button:disabled {
    background: #F5F5F5;
    color : #C3C3C3;
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
      <p class="staticState">EL Wire Sequences Version: <span style="color:blue" id="sequencesVersion">%SEQUENCES_VERSION%</span></p>
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
      </div>

      <h3>LED Array</h3>
      <div class="center">
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
        <input type="button" id="ledMessageSet" value="Set">
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
        Random Sequence: 
        <input type="checkbox" id="randomSequence" name="randomSequence" onclick="toggleRandomSequence()">
        <br>
        Sequence Number:
        <select id="sequenceNumber" name="sequenceNumber" onchange="setSequence()"></select>
        <br>
        Sequence Speed:
        <input type="range", id="sequenceSpeed", min="1", max="500", value="250" onchange="setSequence()"> &nbsp <span id="speed"></span>
      </div>

      <div class="vertical-center">
        <p><button class="green-button" id="save" onclick="saveConfiguration()">Save Configuration</button></p>
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
    websocket.onmessage = onMessage;
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
    document.getElementById('ledMessageSet').addEventListener("click", setLedMsg);
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
    console.log("msgObj: " + JSON.stringify(msgObj));

    elem = document.getElementById("ssid");
    elem.value = msgObj.ssid;

    elem = document.getElementById("password");
    if (msgObj.passwd != null) {
      elem.value = rot47(msgObj.passwd);
    } else {
      elem.value = "";
    }

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

    var rs = (msgObj.randomSequence != 0);
    document.getElementById('randomSequence').checked = rs;
    document.getElementById('sequenceNumber').disabled = rs;

    elem = document.getElementById('ledMessage');
    elem.value = escapeHTML(msgObj.msg);

    elem = document.getElementById('sequenceNumber');
    elem.value = msgObj.sequenceNumber;

    elem = document.getElementById('sequenceSpeed');
    elem.value = msgObj.sequenceSpeed;
    document.getElementById("speed").innerHTML = msgObj.sequenceSpeed;

    document.getElementById("save").disabled = false;
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
    var jsonMsg = JSON.stringify({"msgType": "sequence", sequenceNumber: seqNum, sequenceSpeed: seqSpeed});
    websocket.send(jsonMsg);
  }
  function saveConfiguration() {
    var ssid = document.getElementById("ssid").value;
    var passwd = document.getElementById("password").value;
    var jsonMsg = JSON.stringify({"msgType": "saveConf", "ssid": ssid, "passwd": rot47(passwd)});
    document.getElementById("save").disabled = true;
    websocket.send(jsonMsg);
  }
  function toggleRandomSequence() {
    var randomSeq = document.getElementById("randomSequence").checked;
    document.getElementById("sequenceNumber").disabled = randomSeq;
    var jsonMsg = JSON.stringify({"msgType": "randomSequence", "state": randomSeq});
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
  function rot13(str) {
    return str.split('').map(char => String.fromCharCode(char.charCodeAt(0) + (char.toLowerCase() < 'n' ? 13 : -13))).join('');
  }
  function rot47(x) {
    var s = [];
    for(var i = 0; (i < x.length); i++) {
      var j = x.charCodeAt(i);
      if ((j >= 33) && (j <= 126)) {
        s[i] = String.fromCharCode(33 + ((j + 14) % 94));
      } else {
        s[i] = String.fromCharCode(j);
      }
    }
    return s.join('');
  }
</script>
</body>
</html>)rawliteral";

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

String wifiStatusToString(wl_status_t status) {
  String s = "WIFI_UNKNOWN: " + String(status);
  switch (status) {
    case WL_NO_SHIELD: s = "WIFI_NO_SHIELD";
    case WL_IDLE_STATUS: s = "WIFI_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: s = "WIFI_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: s = "WIFI_SCAN_COMPLETED";
    case WL_CONNECTED: s = "WIFI_CONNECTED";
    case WL_CONNECT_FAILED: s = "WIFI_CONNECT_FAILED";
    case WL_CONNECTION_LOST: s = "WIFI_CONNECTION_LOST";
    case WL_DISCONNECTED: s = "WIFI_DISCONNECTED";
  }
  return(s);
}

void notifyClients() {
  String msg = "{";
  msg += "\"ssid\": \"" + configState.ssid + "\"";
  msg += ", \"passwd\": \"" + configState.passwd + "\"";
  msg += ", \"led\": " + String(configState.ledState);
  msg += ", \"el\": " + String(configState.elState);
  msg += ", \"msg\": \"" + configState.ledMessage + "\"";
  msg += ", \"randomSequence\": " + String(configState.randomSequence);
  msg += ", \"sequenceNumber\": " + String(configState.sequenceNumber);
  msg += ", \"sequenceSpeed\": " + String(configState.sequenceSpeed);
  msg += "}";
  ws.textAll(msg);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    DeserializationError error = deserializeJson(wsMsg, (char *)data);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return;
    }
    if (VERBOSE) {
      println("wsMsg: ");
      serializeJsonPretty(wsMsg, Serial);
    }
    String msgType = String(wsMsg["msgType"]);
    if (msgType.equals("led")) {
      configState.ledState = !configState.ledState;
    } else if (msgType.equals("el")) {
      configState.elState = !configState.elState;
    } else if (msgType.equals("query")) {
      // NOP
    } else if (msgType.equals("ledMsg")) {
      String mode = String(wsMsg["mode"]);
      configState.ledMessage = String(wsMsg["text"]);
      configState.ledMessage.trim();
      int f = wsMsg["fontNum"];
      configState.ledFont = ('0' + f);
      if (mode.equals("set")) {
        leds.message(&configState.ledMessage, configState.ledFont);
      } else if (mode.equals("append")) {
        leds.appendMessage(&configState.ledMessage, configState.ledFont);
      } else {
        Serial.println("Error: unknown mode type: " + mode);
        return;
      }
    } else if (msgType.equals("sequence")) {
      elWires.setSequence(wsMsg["sequenceNumber"], wsMsg["sequenceSpeed"]);
      configState.sequenceNumber = elWires.sequenceNumber();
      configState.sequenceSpeed = elWires.sequenceSpeed();
    } else if (msgType.equals("randomSequence")) {
      elWires.enableRandomSequence(wsMsg["state"] ? true : false);
      configState.randomSequence = elWires.randomSequence();
    } else if (msgType.equals("saveConf")) {
      config["ssid"] = String(wsMsg["ssid"]);
      config["passwd"] = String(wsMsg["passwd"]);
      config["ledState"] = configState.ledState;
      config["ledMessage"] = configState.ledMessage;
      config["ledFont"] = configState.ledFont;
      config["elState"] = configState.elState;
      config["randomSequence"] = configState.randomSequence;
      config["sequenceNumber"] = configState.sequenceNumber;
      config["sequenceSpeed"] = configState.sequenceSpeed;
      if (VERBOSE) {
        println("config: ");
        serializeJsonPretty(config, Serial);
      }
      cs.set(config);
      cs.save();
    } else {
      Serial.println("Error: unknown message type: " + msgType);
      return;
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
    return (configState.ledState ? "ON" : "OFF");
  } else if (var == "EL_STATE") {
    return (configState.ledState ? "ON" : "OFF");
  } else if (var == "SEQUENCES_VERSION") {
    return (elWires.libVersion);
  } else if (var == "NUMBER_OF_SEQUENCES") {
    return (String(elWires.numSequences()));
  }
  return String();
}

void initElWires() {
  println("Init EL Wires");
  elWires.clear();
  elWires.setSequence(config["sequenceNumber"], config["sequenceSpeed"]);
  println("Number of Sequences: " + String(elWires.numSequences()));
  println("Random Sequence Enabled: " + elWires.randomSequence() ? "Yes" : "No");
  println("Sequence Number: " + String(elWires.sequenceNumber()));
  println("Sequence Speed: " + String(elWires.sequenceSpeed()));
}

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
  leds.message(&configState.ledMessage, configState.ledFont);
}

void setup() { 
  delay(500);
  Serial.begin(19200);
  delay(500);
  Serial.println("\nBEGIN");

  //// TMP TMP TMP
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  config = cs.get();
  JsonObject obj = config.as<JsonObject>();
  if (obj.isNull()) {
    Serial.println("No config file, initialize with an empty one");
    cs.initialize();
    deserializeJson(config, "{}");
    cs.set(config);
    cs.save();
  }

  bool dirty = false;
  //// FIXME DRY this up
  if (config.containsKey("ssid") && !config["ssid"].isNull()) {
      configState.ssid = String(config["ssid"]);
  } else {
    config["ssid"] = configState.ssid;
    dirty = true;
  }
  if (config.containsKey("passwd") && !config["passwd"].isNull()) {
    configState.passwd = String(config["passwd"]);
  } else {
    config["passwd"] = configState.passwd;
    dirty = true;
  }
  if (config.containsKey("ledState") && !config["ledState"].isNull()) {
    configState.ledState = config["ledState"];
  } else {
    config["ledState"] = configState.ledState;
    dirty = true;
  }
  if (config.containsKey("ledMessage") && !config["ledMessage"].isNull()) {
    configState.ledMessage = String(config["ledMessage"]);
  } else {
    config["ledMessage"] = configState.ledMessage;
    dirty = true;
  }
  if (config.containsKey("ledFont") && !config["ledFont"].isNull()) {
    configState.ledFont = config["ledFont"];
  } else {
    config["ledFont"] = configState.ledFont;
    dirty = true;
  }
  if (config.containsKey("elState") && !config["elState"].isNull()) {
    configState.elState = config["elState"];
  } else {
    config["elState"] = configState.elState;
    dirty = true;
  }
  if (config.containsKey("randomSequence") && !config["randomSequence"].isNull()) {
    configState.randomSequence = config["randomSequence"];
  } else {
    config["randomSequence"] = configState.randomSequence;
    dirty = true;
  }
  if (config.containsKey("sequenceNumber") && !config["sequenceNumber"].isNull()) {
    configState.sequenceNumber = config["sequenceNumber"];
  } else {
    config["sequenceNumber"] = configState.sequenceNumber;
    dirty = true;
  }
  if (config.containsKey("sequenceSpeed") && !config["sequenceSpeed"].isNull()) {
    configState.sequenceSpeed = config["sequenceSpeed"];
  } else {
    config["sequenceSpeed"] = configState.sequenceSpeed;
    dirty = true;
  }
  if (dirty) {
    cs.set(config);
    cs.save();
  }
  Serial.println("Config:");
  serializeJsonPretty(config, Serial);
  Serial.println("");

  WiFi.mode(WIFI_STA);
  WiFi.begin(configState.ssid, rot47(configState.passwd));
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi.." + wifiStatusToString(WiFi.status()));
    if (i++ > MAX_WIFI_RETRIES) {
      Serial.println("Using fallback WiFi parameters");
      configState.ssid = WLAN_SSID;
      configState.passwd = rot47(WLAN_PASS);
      WiFi.begin(configState.ssid, rot47(configState.passwd));
      delay(1000);
      i = 0;
    }
  }
  Serial.println("\nConnected to " + WiFi.SSID());
  Serial.println("IP address: " + WiFi.localIP().toString());
  Serial.println("RSSI: " + String(WiFi.RSSI()));

  initWebSocket();

  // route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");

  initElWires();
  Serial.println("EL wires function started");

  initLedArray();
  Serial.println("LED array function started");
}

void loop() {
  ws.cleanupClients();

  //// TMP TMP TMP
  digitalWrite(ledPin, configState.ledState);

  if (configState.elState) {
    elWires.run();
  }

  leds.enableDisplay(configState.ledState);
  if (configState.ledState) {
    leds.run();
  }
};
