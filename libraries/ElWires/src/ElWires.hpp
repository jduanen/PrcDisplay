/*
* Library that drives EL wires with different patterns and speeds
*
* N.B. The patterns are generated by tools/seqgen.c
*/

ElWires::ElWires() {
	_numElWires = NUM_EL_WIRES;
	_numSequences = NUM_SEQUENCES;

    _prcd.pinMode(P0, OUTPUT, HIGH);
    _prcd.pinMode(P1, OUTPUT, HIGH);
    _prcd.pinMode(P2, OUTPUT, HIGH);
    _prcd.pinMode(P3, OUTPUT, HIGH);
    _prcd.pinMode(P4, OUTPUT, HIGH);
    _prcd.pinMode(P5, OUTPUT, HIGH);
    _prcd.pinMode(P6, OUTPUT, HIGH);
    _prcd.pinMode(P7, OUTPUT, HIGH);
    _prcd.begin();

    clear();

    // use floating input as source of randomness
    randomSeed(analogRead(UNUSED_ANALOG));
};

unsigned short ElWires::numSequences() {
	return _numSequences;
};

bool ElWires::randomSequence() {
	return _randomSequence;
};

void ElWires::enableRandomSequence(bool enable) {
	_randomSequence = enable;
};

unsigned short  ElWires::sequenceNumber() {
	return _sequenceNumber;
};

unsigned int ElWires::sequenceSpeed() {
	return _sequenceSpeed;
};

void ElWires::setSequence(unsigned short number, unsigned int speed) {
	_sequenceNumber = number;
	_sequenceSpeed = speed;
};

void ElWires::clear() {
    for (int wire = 0; (wire < _numElWires); wire++) {
    	_prcd.digitalWrite(wire, HIGH);
    }
};

void ElWires::writeAll(byte values) {
	PCF8574::DigitalInput digitalInput;

    digitalInput.p7 = bitRead(values, 7);
    digitalInput.p6 = bitRead(values, 6);
    digitalInput.p5 = bitRead(values, 5);
    digitalInput.p4 = bitRead(values, 4);
    digitalInput.p3 = bitRead(values, 3);
    digitalInput.p2 = bitRead(values, 2);
    digitalInput.p1 = bitRead(values, 1);
    digitalInput.p0 = bitRead(values, 0);

    _prcd.digitalWriteAll(digitalInput);
};

void ElWires::run() {
	int wireEnables;

	if ((_randomSequence != _lastRandomSequence) || 
		(_sequenceNumber != _lastSequenceNumber) || 
		(_sequenceSpeed != _lastSequenceSpeed)) {
		_delayCycles = 0;
	}

	if (_delayCycles > 0) {
		_delayCycles--;
		return;
	} else {
		_delayCycles = _sequenceSpeed;
	}

	if (_randomSequence == true) {
		wireEnables = random(0, 255);
	} else {
		if ((_sequenceNumber != _lastSequenceNumber) ||
			(_seqIndx >= sequenceDefinitions[_sequenceNumber].length)) {
			_seqIndx = 0;
		}
		wireEnables = patterns[sequenceDefinitions[_sequenceNumber].offset + _seqIndx++];
	}
	if (false) {
		Serial.println("EL: 0x" + String(wireEnables, HEX));
	}
	writeAll(wireEnables);

	_lastRandomSequence = _randomSequence;
	_lastSequenceNumber = _sequenceNumber;
	_lastSequenceSpeed = _sequenceSpeed;
};

