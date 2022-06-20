/*
 *
 * LED Array Driver Fonts test
 * 
 * TODO
 *  - objectify this whole thing and make a library out of it
 *  - Add per-message blink option (using the SRs' OE bits)
 *  - Combine multiple fonts and provide per-character font selection (parallel (bitmap) vector?)
 *  - Made symbols font
 */

#include <ShiftRegister74HC595.h>
#include "fonts.h"


#define DATA_PIN        14
#define SRCLK_PIN       12
#define RCLK_PIN        13

#define NUM_SR          4
#define NUM_ROWS        7
#define NUM_COLS        21

#define STD_WAIT        100 //50
#define LONG_WAIT       2000

#define WIDE_FONT           0
#define SKINNY_FONT         1
#define VERY_SKINNY_FONT    2


// change this variable (potentially at runtime) to change scroll speed
int stdWaitCycles = STD_WAIT;

ShiftRegister74HC595<NUM_SR> sr(DATA_PIN, SRCLK_PIN, RCLK_PIN);

// active high bitmap, bits[20:0] are row pixels, [0,0] is upper left corner
uint32_t frameBuffer[NUM_ROWS];

// string to be displayed
String message;

int loopCount = 0;
int curChar = 0;
byte curCol = 0;
int waitCycles = STD_WAIT;
int fontNumber = SKINNY_FONT;


void clearFB() {
  for (int i = 0; i < NUM_ROWS; i++) {
    frameBuffer[i] = 0x00;
  }
}

void fillFB() {
  for (int i = 0; i < NUM_ROWS; i++) {
    frameBuffer[i] = 0xFFFFFFFF;
  }
}

void checkerFB() {
  for (int i = 0; i < NUM_ROWS; i++) {
    if (i & 0x1) {
      frameBuffer[i] = 0x55555555;
    } else {
      frameBuffer[i] = 0xAAAAAAAA;
    }
  }
}

void writeFB(char *strPtr) {
  for (int j = 0; j < strlen(strPtr); j++) {
    char c = strPtr[j];
    int columnMask = ((1 << fonts[fontNumber][c].columns) - 1);
    Serial.println("CM: 0x" + String(columnMask, HEX));
    for (int i = 0; i < NUM_ROWS; i++) {
      frameBuffer[i] = (frameBuffer[i] << (fonts[fontNumber][c].columns + 1)) | (fonts[fontNumber][c].code[i] & columnMask);
      Serial.println("FB: 0x" + String(frameBuffer[i], HEX));
    }
  }
}

void scrollMessage() {
  char c = message.charAt(curChar);
  printf("C: %c\n", c);

  for (int i = 0; i < NUM_ROWS; i++) {
    frameBuffer[i] = (frameBuffer[i] << 1) | ((fonts[fontNumber][c].code[i] >> ((fonts[fontNumber][c].columns - 1) - curCol)) & 0x01);
  }
  curCol++;
  if (curCol > fonts[fontNumber][c].columns) {
    // TODO insert a blank column (spacer)
    curCol = 0;
    curChar++;
  }
  if (curChar > message.length()) {
    curChar = 0;
    waitCycles = LONG_WAIT;
  }
}

void clearDisplay() {
  uint8_t srValues[NUM_SR] = {0,};

  for (int i = 0; i < NUM_ROWS; i++) {
    sr.setAll(srValues);
  }
}

void scanDisplay() {
  int rowSelect;
  // layout: [nc[3:0], rows[6:3]], [rows[2:0], cols[20:16]], [cols[15:8]], [cols[7:0]]
  uint8_t srValues[NUM_ROWS][NUM_SR];

  // scan all of the FB's lines out to the display
  for (int i = 0; i < NUM_ROWS; i++) {
    rowSelect = (1 << i);
    srValues[i][0] = ~(frameBuffer[i] & 0xFF);
    srValues[i][1] = ~((frameBuffer[i] >> 8) & 0xFF);
    srValues[i][2] = ((rowSelect & 0x07) << 5) | (~(frameBuffer[i] >> 16) & 0x1F);
    srValues[i][3] = ((rowSelect >> 3) & 0x0F);

    sr.setAll(srValues[i]);
  }
}

void setup() { 
  delay(1000);
  Serial.begin(19200);
  Serial.println("BEGIN");

  fillFB();
  for (int i = 0; i < 500; i++) {
    scanDisplay();
  }
  checkerFB();
  for (int i = 0; i < 500; i++) {
    scanDisplay();
  }
  clearFB();
  for (int i = 0; i < 500; i++) {
    scanDisplay();
  }
  clearFB();

  stdWaitCycles = STD_WAIT;
  fontNumber = SKINNY_FONT;
  message = String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
}

void loop() {
  if ((loopCount % waitCycles) == 0) {
    waitCycles = stdWaitCycles;
    scrollMessage();
  }
  scanDisplay();
  loopCount++;
}
