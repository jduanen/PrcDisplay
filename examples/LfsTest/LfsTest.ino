/*
 * LFS-based state save/restore test
 */

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>


#define STATE_FILENAME  "/state.json"


void setup() {
    delay(500);
    Serial.begin(19200);
    delay(500);
    Serial.println("\nBEGIN");

    if (!LittleFS.begin()) {
        Serial.println("Error: occurred while mounting LittleFS");
        return;
    }
  
    File file = LittleFS.open(STATE_FILENAME, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
  
    Serial.println("File Content:");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void loop() {
}
