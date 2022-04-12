/*
 * LED Array Driver test
 * 
 */

#include <ShiftRegister74HC595.h>


#define ROW_DWELL_TIME  50

#define DATA_PIN        2
#define SRCLK_PIN       4
#define RCLK_PIN        5

#define NUM_SR          4
#define NUM_ROWS        7
#define NUM_COLS        21


ShiftRegister74HC595<NUM_SR> sr(DATA_PIN, SRCLK_PIN, RCLK_PIN);

uint32_t frameBuffer[NUM_ROWS];  // active high bitmap, bits[20:0] are row pixels, [0,0] is upper left corner

void clearFB() {
  int i;
  for (i = 0; i < NUM_ROWS; i++) {
    frameBuffer[i] = 0x00;
  }
}

void fillFB() {
  int i;
  for (i = 0; i < NUM_ROWS; i++) {
    frameBuffer[i] = 0xFF;
  }
}

void checkerFB() {
  int i;
  for (i = 0; i < NUM_ROWS; i++) {
    if (i & 0x1) {
      frameBuffer[i] = 0x55555555;
    } else {
      frameBuffer[i] = 0xAAAAAAAA;
    }
  }
}

void setup() { 
  delay(3000);
  Serial.begin(19200);
  Serial.println("BEGIN");
  delay(1000);

  checkerFB();
}

void loop() {
  int i, rowSelect;
  // layout: [nc[3:0], rows[6:3]], [rows[2:0], cols[20:16]], [cols[15:8]], [cols[7:0]]
  uint8_t srValues[NUM_ROWS][NUM_SR];

  // scan lines
  for (i = 0; i < NUM_ROWS; i++) {
    rowSelect = (1 << i);
    srValues[i][0] = ~(frameBuffer[i] & 0xFF);
    srValues[i][1] = ~((frameBuffer[i] >> 8) & 0xFF);
    srValues[i][2] = ((rowSelect & 0x07) << 5) | (~(frameBuffer[i] >> 16) & 0x1F);
    srValues[i][3] = ((rowSelect >> 3) & 0x0F);

    sr.setAll(srValues[i]);
  }
}
