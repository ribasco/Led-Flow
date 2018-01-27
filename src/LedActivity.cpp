#include "Led.h"

namespace LedLib {
LedActivity& LedActivity::repeat(uint16_t repeat) {
	_repeat = repeat;
	return *this;
}

LedActivity& LedActivity::delay(uint32_t interval) {
	LedPinConfig cfg = LED_CONFIG_DELAY;
	cfg.interval = interval;
	addStep(cfg);
	return *this;
}

LedActivity& LedActivity::on(void) {
	LedPinConfig cfg = LED_CONFIG_ON;
	addStep(cfg);
	return *this;
}

LedActivity& LedActivity::on(uint8_t brightnessPct) {
	LedPinConfig cfg = LED_CONFIG_ON;
	cfg.startValue = brightnessPct;
	cfg.min = 0;
	cfg.max = brightnessPct;
	addStep(cfg);
	return *this;
}

LedActivity& LedActivity::on(uint8_t brightnessPct, uint32_t color) {
	LedPinConfig cfg = LED_CONFIG_ON;
	cfg.startValue = brightnessPct;
	cfg.min = 0;
	cfg.max = brightnessPct;
	cfg.color = color;
	addStep(cfg);
	return *this;
}

LedActivity& LedActivity::off(void) {
	//LedPinConfig cfg = LED_CONFIG_OFF;
	addStep(LED_CONFIG_OFF);
	return *this;
}

LedActivity& LedActivity::blink(bool smooth, uint16_t repeat, uint32_t interval) {
	LedPinConfig cfg = (smooth) ? LED_CONFIG_BLINK_SMOOTH : LED_CONFIG_BLINK;
	cfg.interval = interval;
	cfg.repeat = repeat;
	addStep(cfg);
	return *this;
}

LedActivity& LedActivity::blink(bool smooth, uint16_t repeat, uint32_t interval, uint32_t color) {
	LedPinConfig cfg = (smooth) ? LED_CONFIG_BLINK_SMOOTH : LED_CONFIG_BLINK;
	cfg.interval = interval;
	cfg.repeat = repeat;
	cfg.color = color;
	addStep(cfg);
	return *this;
}

LedActivity& LedActivity::fade(uint32_t from, uint32_t to, uint16_t repeat, uint32_t interval) {
	LedPinConfig cfg;
	if (to > from) {
		cfg = LED_CONFIG_FADE_IN;
		cfg.min = from;
		cfg.max = to;
	} else if (from > to) {
		cfg = LED_CONFIG_FADE_OUT;
		cfg.min = to;
		cfg.max = from;
	} else {
		cfg = LED_CONFIG_ON;
	}
	cfg.repeat = repeat;
	cfg.interval = interval;
	cfg.color = LED_DEFAULT_COLOR;
	addStep(cfg);
	return *this;
}

LedActivity& LedActivity::custom(LedPinConfig& config) {
	addStep(config);
	return *this;
}

// Private
LedActivity* LedActivity::addStep(LedPinConfig step) {
	if (_steps != NULL && (_writeIndex <= (_count - 1))) {
		_steps[_writeIndex++] = step;
	}
	return this;
}

int16_t& LedActivity::count(void) {
	return _count;
}

uint16_t& LedActivity::repeat(void) {
	return _repeat;
}

LedPinConfig* LedActivity::step(int8_t index) {
	if (index > (_count - 1)) {
		return NULL;
	}
	return &_steps[index];
}
} // namespace LedLib
