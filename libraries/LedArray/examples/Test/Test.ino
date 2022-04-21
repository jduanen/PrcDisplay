/*
 *
 * LED Array Driver Library test
 * 
 */

#include <Arduino.h>
#include <LedArray.h>


#define TEST_NUMBER         1

#define DATA_PIN            2
#define SRCLK_PIN           4
#define RCLK_PIN            5

#define NUM_SR              4
#define NUM_ROWS            7
#define NUM_COLS            21

#define STD_WAIT            50

#define WIDE_FONT           '0'
#define SKINNY_FONT         '1'
#define VERY_SKINNY_FONT    '2'
#define SYMBOLS_FONT        '3'


LedArray<NUM_SR> leds(DATA_PIN, SRCLK_PIN, RCLK_PIN, NUM_ROWS, NUM_COLS, NUM_SR, STD_WAIT);

int loopCnt = 0;


void setup() { 
  delay(1000);
  Serial.begin(19200);
  Serial.println("BEGIN");

  //// TODO get and print available font info

  String msg;
  switch (TEST_NUMBER) {
    case 0:
      msg = String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
      leds.message(&msg, SKINNY_FONT);
    break;
    case 1:
      leds.message("ABCDabcd", WIDE_FONT);
      leds.appendMessage("EFGHefgh", VERY_SKINNY_FONT);
      leds.message("IJKLijkl", SKINNY_FONT);
    break;
    case 2:
      leds.message("ABCDEFGHIJKLM", SYMBOLS_FONT);
    break;
    default:
      Serial.println("Invalid Test Number: " + String(TEST_NUMBER));
      break;
  }
}

void loop() {
  leds.run();
  loopCnt++;
}
