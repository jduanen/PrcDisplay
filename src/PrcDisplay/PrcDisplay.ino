/***************************************************************************
*
* PrcDisplay application
* 
* Drives eight 9' EL wires using the AFD Driver board and a 7x21 LED display
*  using four 74HC595 shift registers.
*
* Notes:
*  - I2C defaults: SDA=GPIO4 -> D2, SCL=GPIO5 -> D1
*
* TODO
*  - Add per-message blink option (using the SRs' OE bits)
*  - Fill out symbols font
*  - Clean up fonts
*  - Add web interface and OTA capabilities
*
****************************************************************************/

#include <Arduino.h>
#include <PCF8574.h>
#include <ShiftRegister74HC595.h>
#include <LedArray.h>

#define EL_WIRES            1
#define LED_ARRAY           1

#define VERBOSE             0

int loopCnt = 0;

#ifdef EL_WIRES
#define I2C_BASE_ADDR       0x38    // PCF8574A
//#define I2C_BASE_ADDR       0x20    // PCF8574
#define READ_ADDR           0x4F
#define WRITE_ADDR          0x4E

#define NUM_EL_WIRES        8

//#define SCL_PIN             1
//#define SDA_PIN             2

PCF8574 prcd = PCF8574(I2C_BASE_ADDR);

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
  if (VERBOSE) {
    Serial.print("writeAllWires: 0x" +  String(values, HEX) + "; ");
  }
}
#endif /*EL_WIRES*/

#ifdef LED_ARRAY
#define TEST_NUMBER         1   //// TMP TMP TMP

#define DATA_PIN            14  // D5
#define SRCLK_PIN           12  // D6
#define RCLK_PIN            13  // D7
#define OE_PIN              15  // D8

#define NUM_SR              4
#define NUM_ROWS            7
#define NUM_COLS            21

#define STD_WAIT            50

#define WIDE_FONT           '0'
#define SKINNY_FONT         '1'
#define VERY_SKINNY_FONT    '2'
#define SYMBOLS_FONT        '3'

LedArray<NUM_SR> leds(DATA_PIN, SRCLK_PIN, RCLK_PIN, NUM_ROWS, NUM_COLS, NUM_SR, STD_WAIT);

//// FIXME
void initLedArray() {
  String msg = String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

  if (VERBOSE) {
    Serial.println("Fonts Version: " + String(FONTS_VERSION));
    Serial.println("Number of Fonts: " + String(NUM_FONTS));
    Serial.print("Font Names: [");
    for (int i = 0; (i < NUM_FONTS); i++) {
      Serial.print(fontNames[i] + String(", "));
    }
    Serial.println("]");
  }

  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);

  leds.clear();
  if (VERBOSE) {
    Serial.println("Running Test #" + String(TEST_NUMBER));
  }
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
      Serial.println("Error: Invalid Test Number: " + String(TEST_NUMBER));
      break;
  }
}

void enableLedArray(bool enable) {
  digitalWrite(OE_PIN, enable ? HIGH : LOW);
}
#endif /*LED_ARRAY*/

void setup() { 
  delay(500);
  if (VERBOSE) {
    Serial.begin(19200);
    delay(500);
    Serial.println("\nBEGIN");
  }

#ifdef EL_WIRES
  initWires();
#endif /*EL_WIRES*/
#ifdef LED_ARRAY
  initLedArray();
#endif /*LED_ARRAY*/
}

void loop() {
#ifdef EL_WIRES
  byte wireVals = (1 << (loopCnt % 8));
  writeAllWires(wireVals);
  if (VERBOSE) {
    Serial.println("wireVals: 0x" + String(wireVals, HEX));
  }
#endif /*EL_WIRES*/

#ifdef LED_ARRAY
  leds.run();
#endif /*LED_ARRAY*/

  loopCnt++;
  if (VERBOSE) {
    Serial.println("loopcnt: " + String(loopCnt));
  }
};
