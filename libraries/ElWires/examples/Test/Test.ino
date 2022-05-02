/*
* EL Wire Driver Library test
*/

#include <Arduino.h>
#include "sequences.h"
#include "ElWires.h"


ElWires elWires;


void initElWires() {
  Serial.println("Number of Sequences: " + elWires.numSequences());
  Serial.println("Random Sequence Enabled: " + elWires.randomSequence() ? "Yes" : "No");
  Serial.println("Sequence Number: " + elWires.sequenceNumber());
  Serial.println("Sequence Speed: " + elWires.sequenceSpeed());
  elWires.clear();
}

void setup() { 
  delay(500);
  Serial.begin(19200);
  delay(500);
  Serial.println("\nBEGIN");

  initElWires();
  Serial.println("EL wires function started");
}

void loop() {
  elWires.run();
}
