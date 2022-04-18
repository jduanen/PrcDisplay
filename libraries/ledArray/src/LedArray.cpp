/*
* Library that scrolls messages on a small (<32 columns) array of LEDs
*
* N.B. Fonts are built-in and generated by fontgen.c
*/

#include <Arduino.h>
#include "fonts.h"


#define DATA_PIN        2
#define SRCLK_PIN       4
#define RCLK_PIN        5

#define NUM_SR          4
#define NUM_ROWS        7
#define NUM_COLS        21


LedArray::LedArray(byte dataPin, byte srClkPin, byte rClkPin, byte numRows, byte numCols, byte numSRs, int scrollSpeed) {
    stdWaitCycles = scrollSpeed;
    waitCycles = stdWaitCycles;
    numRows = numRows;
    numCols = numCols;
    numSRs = numSRs;
    frameBufferPtr = new uint32_t[numRows];
    srPtr = new ShiftRegister74HC595<numSRs>(dataPin, srClkPin, rClkPin);
}

void LedArray::clear() {
    printf("clear: TBD");
}

void LedArray::fill(uint32_t val) {
    printf("fill: TBD");
}

void LedArray::fill(uint32_t vals[]) {
    for (int i = 0; i < numRows; i++) {
        fill(vals[i]);
    }
}

void LedArray::run() {
    printf("run: TBD");
}

//// TODO add getFonts() -- return number and description of each font

void message(String str, byte fontNumber) {
    assert((fontNumber >= 0) && (fontNumber < NUM_FONTS), "Invalid font number selection");
    printf("message: TBD");
}

void appendMessage(String str, byte fontNumber) {
    assert((fontNumber >= 0) && (fontNumber < NUM_FONTS), "Invalid font number selection");
    printf("appendMessage: TBD");
}

void LedArray::writeToFB(char *strPtr) {
    printf("writeToFB: TBD");
}

void LedArray::scrollMessage() {
    printf("scrollMessage: TBD");
}

void LedArray::scanDisplay() {
    printf("scanDisplay: TBD");
}