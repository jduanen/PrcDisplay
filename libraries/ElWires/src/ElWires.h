/*
* Library that drives EL wires with different patterns and speeds
*/

#pragma once

#include <Arduino.h>
#include <PCF8574.h>


#define EL_WIRE_LIB_VERSION     "1.0"

#define DEF_SEQUENCE_NUMBER 0
#define DEF_SEQUENCE_DELAY  100

#define I2C_BASE_ADDR       0x38    // PCF8574A afd driver
//#define I2C_BASE_ADDR       0x20    // PCF8574 breakout board
#define READ_ADDR           0x4F
#define WRITE_ADDR          0x4E

#define UNUSED_ANALOG       A0


class ElWires {
public:
    String libVersion = EL_WIRE_LIB_VERSION;

	ElWires();
	ElWires(bool delayedStart);
	void start();
	bool randomSequence();
	void enableRandomSequence(bool enable);

	unsigned short numSequences();
	unsigned short sequenceNumber();
	uint32_t sequenceDelay();
	void setSequence(unsigned short number, uint32_t delay);

	void clear();
	void writeAll(byte values);
	unsigned long run();

private:
	PCF8574 _prcd = PCF8574(I2C_BASE_ADDR);

	uint8_t _numElWires;
	unsigned short _numSequences;

	unsigned short _sequenceNumber = DEF_SEQUENCE_NUMBER;
	uint32_t _sequenceDelay = DEF_SEQUENCE_DELAY;

	bool _randomSequence = false;

    unsigned long _nextRunTime = 0;
	unsigned int _seqIndx = 0;

	void _create(bool delayedStart);
};

#include "ElWires.hpp"
