#include "LedFlow.h"

void LedManager::setUpdateInterval(uint32_t interval) {
	_interval = interval;
}

int8_t LedManager::add(Led& led) {
	int idx = findFreeIndex();
	if (idx == LED_INVALID_INDEX) {
		return LED_INVALID_INDEX;
	}
	_leds[idx] = &led;
	return idx;
}

int8_t LedManager::remove(Led& led) {
	for (uint8_t i = 0; i < LEDM_MAX_LEDS; i++) {
		if (_leds[i] != NULL && (_leds[i]->_id == led._id)) {
			_leds[i] = NULL;
			return i;
		}
	}
	return LED_INVALID_INDEX;
}

void LedManager::setValue(uint32_t value) {
	for (uint8_t i = 0; i < LEDM_MAX_LEDS; i++) {
		Led* led = _leds[i];
		if (led != NULL)
			led->setValue(value);
	}
}

void LedManager::updateAllLeds(const uint32_t& now) {
	for (uint8_t i = 0; i < LEDM_MAX_LEDS; i++) {
		Led* led = _leds[i];
		if (led != NULL)
			led->update(now);
	}
}

void LedManager::update(void) {
	uint32_t currentMillis = millis();
	if ((currentMillis - _previousMillis) >= _interval) {
		_previousMillis = currentMillis;
		updateAllLeds(currentMillis);
	}
}

int LedManager::findLedIndex(Led& led) {
	for (int i = 0; i < LEDM_MAX_LEDS; i++) {
		if (_leds[i] != NULL && (_leds[i]->_id == led._id)) {
			_leds[i] = NULL;
			return i;
		}
	}
	return LED_INVALID_INDEX;
}

int LedManager::findFreeIndex(void) {
	for (int i = 0; i < LEDM_MAX_LEDS; i++) {
		if (_leds[i] == NULL)
			return i;
	}
	return LED_INVALID_INDEX;
}

int8_t LedManager::count(void) {
	int8_t ctr = 0;
	for (int i = 0; i < LEDM_MAX_LEDS; i++) {
		if (_leds[i] != NULL)
			ctr++;
	}
	return ctr;
}
