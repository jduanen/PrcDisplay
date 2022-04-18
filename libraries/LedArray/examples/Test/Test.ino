/*
 *
 * LED Array Driver Library test
 * 
 */

#include <LedArray.h>


#define DATA_PIN        2
#define SRCLK_PIN       4
#define RCLK_PIN        5

#define NUM_SR          4
#define NUM_ROWS        7
#define NUM_COLS        21

#define STD_WAIT        100 //50

#define WIDE_FONT           0
#define SKINNY_FONT         1
#define VERY_SKINNY_FONT    2


LedArray leds = LedArray<NUM_SR>(DATA_PIN, SRCLK_PIN, RCLK_PIN, NUM_ROWS, NUM_COLS, NUM_SR, STD_WAIT);


void setup() { 
  delay(1000);
  Serial.begin(19200);
  Serial.println("BEGIN");

  //// TODO get and print available font info

  String msg = String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  leds.message(msg, SKINNY_FONT);
}

void loop() {
  leds.run();
  printf("Loop");
  delay(1000);
}
