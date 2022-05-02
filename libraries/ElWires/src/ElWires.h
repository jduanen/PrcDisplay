/*
* Library that drives EL wires with different patterns and speeds
*/

#pragma once

#include <Arduino.h>
#include <PCF8574.h>


#define EL_WIRE_LIB_VERSION     "1.0"

#define DEF_SEQUENCE_NUMBER 0
#define DEF_SEQUENCE_SPEED  0

#define I2C_BASE_ADDR       0x38    // PCF8574A
//#define I2C_BASE_ADDR       0x20    // PCF8574
#define READ_ADDR           0x4F
#define WRITE_ADDR          0x4E

#define UNUSED_ANALOG       A0


class ElWires {
public:
    String libVersion = EL_WIRE_LIB_VERSION;

	ElWires();
	bool randomSequence();
	void enableRandomSequence(bool enable);

	unsigned short numSequences();
	unsigned short sequenceNumber();
	unsigned int sequenceSpeed();
	void setSequence(unsigned short number, unsigned int speed);

	void clear();
	void writeAll(byte values);
	void run();

private:
	PCF8574 _prcd = PCF8574(I2C_BASE_ADDR);

	uint8_t _numElWires;
	unsigned short _numSequences;

	unsigned int _delayCycles = 0;
	unsigned int _seqIndx = 0;

	bool _randomSequence = false;
	unsigned short _sequenceNumber = DEF_SEQUENCE_NUMBER;
	unsigned int _sequenceSpeed = DEF_SEQUENCE_SPEED;

	bool _lastRandomSequence = _randomSequence;
	unsigned short _lastSequenceNumber = _sequenceNumber;
	unsigned int _lastSequenceSpeed = _sequenceSpeed;
};

#include "ElWires.hpp"
