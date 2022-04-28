/*
* Library that scrolls messages on a small (<32 columns) array of LEDs
*
* N.B.
*  - The timing constants are all scaled to a standard ESP8266 clock
*     * change them when running at different clock speed
*  - Fonts are built-in and generated by fontgen.c
*/

// TODO
//  * fix blink display feature
//  * add blink character feature (make it an attribute like fonts)

#pragma once

#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#define LIB_VERSION     "1.0"
#define LONG_WAIT       1000


template<uint8_t Size>
class LedArray {
public:
    String libVersion = LIB_VERSION;

    LedArray(uint8_t dataPin, uint8_t srClkPin, uint8_t rClkPin, uint8_t oePin, uint8_t numRows, uint8_t numCols, int scrollSpeed);

    void clear();
    void fill(uint32_t val);
    void run();
    void message(char str[], char font);
    void message(String *strPtr, char font);
    void appendMessage(char str[], char font);
    void appendMessage(String *strPtr, char font);
    void enableDisplay(bool enable);
    void blinkDisplay(byte rate);

private:
    int _stdWaitCycles;
    int _waitCycles;

    uint8_t _numRows;
    uint8_t _numCols;

    //// FIXME figure out how to handle this
    ShiftRegister74HC595<Size> *_srPtr;

    // active high bitmap, row pixels start at the LSB, [0,0] is upper left corner
    uint32_t *_frameBufferPtr;

    String _msg = "";
    String _fontNums = "";

    int _loopCount = 0;
    int _curChar = 0;
    uint8_t _curCol = 0;
    bool _oePin;
    bool _displayEnabled = true;
    byte _blinkRate = 0;     // disabled

//    void _message(String str, char font);
    //void writeToFB(char *strPtr);
    void scrollMessage();
    void scanDisplay();
    void clearDisplay();
};

#include "LedArray.hpp"
