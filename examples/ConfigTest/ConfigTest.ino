/*
 * LFS-based configuration state save/restore test
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#define CS_USE_LITTLEFS     true
#include <ConfigStorage.h>


#define CONFIG_PATH     "/config.json"


ConfigStorage cs(CONFIG_PATH);

StaticJsonDocument<512> configDoc;


void setup() {
    delay(500);
    Serial.begin(19200);
    delay(500);
    Serial.println("\nBEGIN");

    configDoc = cs.get();
    JsonObject obj = configDoc.as<JsonObject>();
    if (obj.isNull()) {
        Serial.println("Initialize config file");
        cs.initialize();
        deserializeJson(configDoc, "{\"foo\": 0, \"bar\":\"baz\"}");
        cs.set(configDoc);
        cs.save();
    } else {
        Serial.println("Config Doc:");
        serializeJsonPretty(configDoc, Serial);
        Serial.println("");
    }

    int val = int(configDoc["foo"]) + 1;

    if (val > 3) {
        Serial.println("Clear Config file");
        cs.initialize();
    } else {
        Serial.println("Update Config file");
        deserializeJson(configDoc, "{\"foo\": " + String(val) + ", \"bar\":\"baz\"}");
        cs.set(configDoc);
    }
    cs.save();

    Serial.println("Re-read Config file");
    configDoc = cs.get();
    obj = configDoc.as<JsonObject>();
    if (obj.isNull()) {
        Serial.println("Empty config file");
    } else {
        Serial.println("New Config File Content:");
        serializeJsonPretty(configDoc, Serial);
    }
}

void loop() {
}
