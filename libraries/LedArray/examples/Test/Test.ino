/*
 *
 * LED Array Driver Library test
 * 
 */

#include <Arduino.h>
#include "fonts.h"
#include <LedArray.h>


#define TEST_NUMBER         0

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

int loopCnt = 0;


void setup() { 
  String msg = String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

  delay(1000);
  Serial.begin(19200);
  Serial.println("\nBEGIN");

  Serial.println("Fonts Version: " + String(FONTS_VERSION));
  Serial.println("Number of Fonts: " + String(NUM_FONTS));
  Serial.print("Font Names: [");
  for (int i = 0; (i < NUM_FONTS); i++) {
    Serial.print(fontNames[i] + String(", "));
  }
  Serial.println("]");

  leds.clear();
  Serial.println("Running Test #" + String(TEST_NUMBER));
  switch (TEST_NUMBER) {
    case 0:
      leds.message(msg, SKINNY_FONT);
    break;
    case 1:
      leds.message("Wide FONT; ", WIDE_FONT);
      leds.appendMessage("Skinny FONT; ", SKINNY_FONT);
      leds.appendMessage("Very Skinny FONT; ", VERY_SKINNY_FONT);
      leds.appendMessage("Symbols FONT: ", WIDE_FONT);
      leds.appendMessage("ABCDEFGHIJKLMNOPQRSTU...", SYMBOLS_FONT);
    break;
    case 2:
      leds.message(msg, WIDE_FONT);
    break;
    case 3:
      leds.message(msg, SKINNY_FONT);
    break;
    case 4:
      leds.message(msg, VERY_SKINNY_FONT);
    break;
    case 5:
      leds.message(msg, SYMBOLS_FONT);
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
