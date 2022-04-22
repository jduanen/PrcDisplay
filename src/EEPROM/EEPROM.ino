#include <Wire.h>

#define ADDR_Ax 0b000 //A2, A1, A0
#define ADDR (0b1010 << 3) + ADDR_Ax


int addr = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  Wire.begin();
}

#define NUM_BYTES   4

void loop() {
  byte val;
  String s = "";
  int i, j;

  s = "0x" + String(addr);
  for (i = 0; i < NUM_BYTES; i++) {
    val = readI2CByte(addr++);
    for (j = 7; j >= 0; j--) {
      if ((val >> j) & 1) {
        s += "X";
      } else {
        s += "_";
      }
    }
  }
  Serial.println(s);
}

void writeI2CByte(byte data_addr, byte data){
  Wire.beginTransmission(ADDR);
  Wire.write(data_addr);
  Wire.write(data);
  Wire.endTransmission();
}

byte readI2CByte(byte data_addr){
  byte data = NULL;
  Wire.beginTransmission(ADDR);
  Wire.write(data_addr);
  Wire.endTransmission();
  Wire.requestFrom(ADDR, 1); //retrieve 1 returned byte
  delay(1);
  if(Wire.available()){
    data = Wire.read();
  }
  return data;
}
