#include "LedFlow.h"

LedRgb::LedRgb(int8_t redPin, int8_t greenPin, int8_t bluePin) : Led() {
	_redPin = createPin(IDX_LED_RED, redPin);
	_greenPin = createPin(IDX_LED_GREEN, greenPin);
	_bluePin = createPin(IDX_LED_BLUE, bluePin);
}

//@Override
void LedRgb::setValue(uint32_t value) {
	Led::setValue(_redPin, R_BYTE(value));
	Led::setValue(_greenPin, G_BYTE(value));
	Led::setValue(_bluePin, B_BYTE(value));
}

void LedRgb::setValueRed(uint8_t value) {
	Led::setValue(_redPin, value);
}

void LedRgb::setValueGreen(uint8_t value) {
	Led::setValue(_greenPin, value);
}

void LedRgb::setValueBlue(uint8_t value) {
	Led::setValue(_bluePin, value);
}

uint32_t LedRgb::getValue(void) {
	return RGB(getValueRed(), getValueGreen(), getValueBlue());
}

uint8_t LedRgb::getValueRed(void) {
	return Led::getValue(_redPin);
}

uint8_t LedRgb::getValueGreen(void) {
	return Led::getValue(_greenPin);
}

uint8_t LedRgb::getValueBlue(void) {
	return Led::getValue(_bluePin);
}

LedPin& LedRgb::getRedPin(void) {
	return *_redPin;
}

LedPin& LedRgb::getGreenPin(void) {
	return *_greenPin;
}

LedPin& LedRgb::getBluePin(void) {
	return *_bluePin;
}

bool LedRgb::preProcessPin(LedPin& pin, LedPinConfig* config,
		LedActivityEvent& event) {
	//Since we are dealing with three separate pins for an RGB led,
	//we need to make three separate copies of the base configuration (of the current activity)
	//for each RGB Pin
	_tmp[pin.index()] = *config;
	LedPinConfig* cfg = &_tmp[pin.index()];

	if (cfg->color == 0)
		cfg->color = LED_DEFAULT_COLOR;

	if (pin.index() == IDX_LED_RED) {
		cfg->min = 0;
		cfg->max = R_BYTE(cfg->color);
	} else if (pin.index() == IDX_LED_GREEN) {
		cfg->min = 0;
		cfg->max = G_BYTE(cfg->color);
	} else if (pin.index() == IDX_LED_BLUE) {
		cfg->min = 0;
		cfg->max = B_BYTE(cfg->color);
	}

	pin.setConfig(cfg);

	return true;
}
