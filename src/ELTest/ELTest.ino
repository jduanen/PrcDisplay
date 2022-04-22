/***************************************************************************
*
* Test program for an EL Wire Driver Using the AFD Driver Board
*
* Notes:
*   - NodeMCUv3 I2C defaults: SDA=4 -> D2, SCL=5 -> D1
*   - NodeMCUv3 SPI defaults: MCLK -> D5, MISO -> D6, MOSI -> D7, SS/CS -> D8
*
****************************************************************************/

#include "Arduino.h"
#include "PCF8574.h"


#define VERBOSE         1

#define I2C_BASE_ADDR   0x38    // PCF8574A
//#define I2C_BASE_ADDR   0x20    // PCF8574
#define READ_ADDR       0x4F
#define WRITE_ADDR      0x4E

#define NUM_WIRES       8

int indx = 0;

PCF8574 prcd = PCF8574(I2C_BASE_ADDR);


void writeAll(byte values) { 
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
  if (VERBOSE) {
    Serial.print("writeAll: 0x" +  String(values, HEX) + "; ");
  }
}

void setup() { 
  delay(500);
  Serial.begin(19200);
  delay(500);

  prcd.pinMode(P0, OUTPUT, HIGH);
  prcd.pinMode(P1, OUTPUT, HIGH);
  prcd.pinMode(P2, OUTPUT, HIGH);
  prcd.pinMode(P3, OUTPUT, HIGH);
  prcd.pinMode(P4, OUTPUT, HIGH);
  prcd.pinMode(P5, OUTPUT, HIGH);
  prcd.pinMode(P6, OUTPUT, HIGH);
  prcd.pinMode(P7, OUTPUT, HIGH);
  prcd.begin();

  for (int wire = 0; (wire < NUM_WIRES); wire++) {
    prcd.digitalWrite(wire, HIGH);
  }

  if (VERBOSE) {
    Serial.println("Start");
  }
}

void loop() {
  writeAll(indx++);
  Serial.println("loopcnt: " + String(indx));
  delay(100);
};
