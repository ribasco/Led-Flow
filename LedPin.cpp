#include "LedFlow.h"

namespace LedFlow {

uint8_t LedPin::index(void) {
	return _index;
}

void LedPin::setPin(int8_t pin) {
	_pin = pin;
}

int8_t LedPin::getPin(void) {
	return _pin;
}

void LedPin::setValue(uint8_t value) {
	if (_value != value) {
		_invalidated = true;
		getLed()->firePinActivityEvent(*this, LedActivityEvent::INVALIDATED);
	}
	_value = value;
}

uint8_t LedPin::getValue(void) {
	_invalidated = false;
	return _value;
}

void LedPin::setStepDirection(LedStepDirection direction) {
	_direction = direction;
}

LedStepDirection LedPin::getStepDirection(void) {
	return _direction;
}

bool LedPin::isInvalidated(void) {
	return _invalidated;
}

uint32_t LedPin::getIteration(void) {
	return _iteration;
}

//==========================================
//Private Methods
//==========================================

bool LedPin::maxLimitReached(void) {
	return maxLimitReached(_value);
}

bool LedPin::maxLimitReached(uint8_t value) {
	return value >= _config->max;
}

bool LedPin::minLimitReached(void) {
	return minLimitReached(_value);
}

bool LedPin::minLimitReached(uint8_t value) {
	return value <= _config->min;
}

void LedPin::setConfig(LedPinConfig* config) {
	if (*_config != *config)
		getLed()->firePinActivityEvent(*this, LedActivityEvent::CONFIG_CHANGED);
	_config = config;
	reset();
}

LedPinConfig* LedPin::getConfig(void) {
	return _config;
}

void LedPin::reset(void) {
	setStepDirection(_config->startDirection);
	setValue(_config->startValue);
}

bool LedPin::isActive(void) {
	return _active;
}

void LedPin::setActive(bool active) {
	if (!_active && active)
		getLed()->firePinActivityEvent(*this, LedActivityEvent::STARTED);
	_active = active;
}

void LedPin::complete(void) {
	_iteration = 0;
	_cycle = 0;
	setActive(false);
	getLed()->firePinActivityEvent(*this, LedActivityEvent::COMPLETED);
}

void LedPin::setLed(Led* led) {
	_led = led;
}

Led* LedPin::getLed(void) {
	return _led;
}
} //namespace
