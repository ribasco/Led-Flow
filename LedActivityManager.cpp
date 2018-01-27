/*
 * LedActivityManager.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: Rafael Ibasco
 */

#include "Led.h"

namespace LedLib {

void LedActivityManager::setActivity(LedActivity* activity) {
	_activity = activity;
	reset();
}

LedActivity& LedActivityManager::getActivity(void) {
	return *_activity;
}

//NEW METHODS
void LedActivityManager::reset(void) {
	for (uint8_t i = 0; i < sizeof(_readIndex); i++) {
		_readIndex[i] = -1;
	}
}

void LedActivityManager::reset(LedPin& pin) {
	if (!isValidPin(pin))
		return;
	_readIndex[pin.index()] = -1;
}

LedPinConfig* LedActivityManager::current(LedPin& pin) {
	if (isValidPin(pin) && _readIndex[pin.index()] > -1)
		return _activity->step(_readIndex[pin.index()]);
	return NULL;
}

LedPinConfig* LedActivityManager::next(LedPin& pin) {
	if (!isValidPin(pin))
		return NULL;
	if (_readIndex[pin.index()] > (_activity->count() - 1))
		_readIndex[pin.index()] = -1;
	return _activity->step(++_readIndex[pin.index()]);
}

int8_t LedActivityManager::currentIndex(LedPin& pin) {
	if (!isValidPin(pin))
		return -1;
	return _readIndex[pin.index()];
}

bool LedActivityManager::hasNext(LedPin& pin) {
	return (isValidPin(pin)
			&& (_readIndex[pin.index()] + 1) <= (_activity->count() - 1)) ?
			true : false;
}

uint16_t LedActivityManager::repeat(LedPin& pin) {
	if (!isValidPin(pin))
		return 0;
	return _repeatCtr[pin.index()];
}

uint16_t LedActivityManager::repeatIncrement(LedPin& pin) {
	if (!isValidPin(pin))
		return 0;
	return ++_repeatCtr[pin.index()];
}

void LedActivityManager::repeatSet(uint16_t value) {
	for (uint8_t i = 0; i < sizeof(_readIndex); i++) {
		_repeatCtr[i] = value;
	}
}

void LedActivityManager::repeatSet(LedPin& pin, uint16_t value) {
	if (!isValidPin(pin))
		return;
	_repeatCtr[pin.index()] = value;
}
}
