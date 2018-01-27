#include "Led.h"

namespace LedLib {
LedSingle::LedSingle(int8_t pin) :
		Led() {
	_pin = createPin(IDX_LED, pin);
}

void LedSingle::setValue(uint32_t value) {
	Led::setValue(_pin, W_BYTE(value));
}

//@Override
uint32_t LedSingle::getValue(void) {
	return Led::getValue(_pin);
}

LedPin& LedSingle::getPin(void) {
	return *_pin;
}

/*void LedSingle::update(const uint32_t& now) {
	//Continue updating pin state
	if (_pin != NULL && (_pin->getPin() > -1 && _pin->isActive())) {
		updatePinState(_pin, now);
		writePinState(_pin);
	}
}*/
} // namespace LedLib
