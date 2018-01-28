#include "LedFlow.h"

namespace LedFlow {

LedPinConfig LED_CONFIG_DEFAULT = {
LED_DEFAULT_VALUE, //start value
		LED_DEFAULT_STEPDIR, //start direction
LED_DEFAULT_STEPUP,//step up offset
LED_DEFAULT_STEPDN,//step down offset
LED_PWM_MIN,//min value
LED_PWM_MAX,//max value
LED_DEFAULT_OSCILLATE,//oscillate
LED_DEFAULT_SMOOTH,//smooth
LED_DEFAULT_INTERVAL,//interval
LED_DEFAULT_REPEAT,//repeat
LED_DEFAULT_CPI,//cycles per iteration
false//override
};

LedPinConfig LED_CONFIG_DELAY = { 0, //start value
		LedStepDirection::STEP_NONE, //start direction
		LED_DEFAULT_STEPUP, //step up offset
		LED_DEFAULT_STEPDN, //step down offset
		LED_PWM_MIN, //min value
		LED_PWM_MAX, //max value
		false, //oscillate
		false, //smooth
		1000, //interval
		1, //repeat
		1,  //cycles per iteration
		false //override value
		};

LedPinConfig LED_CONFIG_FADE_IN = { 0, //start value
		LedStepDirection::STEP_UP, //start direction
		0,  //step up offset
		0, //step down offset
		LED_PWM_MIN, //min value
		LED_PWM_MAX, //max value
		false, //oscillate
		true, //smooth
		50,  //interval
		5, //repeat
		1, //cycles per iteration
		false //override value
		};

LedPinConfig LED_CONFIG_FADE_OUT = { 255, //value
		LedStepDirection::STEP_DOWN, //direction
		0, //step-up
		0, //step-down
		LED_PWM_MIN, //min
		LED_PWM_MAX, //max
		false, //oscillate
		true, //smooth
		50, //interval
		5, //repeat
		1,  //cycles per iteration
		false //override value
		};

LedPinConfig LED_CONFIG_ON = {
LED_PWM_MAX, //value
		LedStepDirection::STEP_UP, //direction (set this to STEP_DOWN to trigger the complete event)
		0, //step-up
		0, //step-down
		LED_PWM_MIN, //min
		LED_PWM_MAX, //max
		false, //oscillate
		false, //smooth
		50, //interval
		1, //repeat
		1,  //cycles per iteration
		true //override value
		};

LedPinConfig LED_CONFIG_OFF = {
LED_PWM_MIN, //value
		LedStepDirection::STEP_DOWN, //direction
		0, //step-up
		0, //step-down
		LED_PWM_MIN, //min
		LED_PWM_MAX, //max
		false, //oscillate
		false, //smooth
		50, //interval
		1, //repeat
		1,  //cycles per iteration
		true //override value
		};

LedPinConfig LED_CONFIG_BLINK = { 0, //value
		LedStepDirection::STEP_UP, //direction
		0, //step-up
		0, //step-down
		LED_PWM_MIN, //min
		LED_PWM_MAX, //max
		true, //oscillate
		false, //smooth
		500, //interval
		20, //repeat
		2,  //cycles per iteration
		false //override value
		};

LedPinConfig LED_CONFIG_BLINK_SMOOTH = { 0, //value
		LedStepDirection::STEP_UP, //direction
		0, //step-up
		0, //step-down
		LED_PWM_MIN, //min
		LED_PWM_MAX, //max
		true, //oscillate
		true, //smooth
		0, //interval
		0, //repeat
		2,  //cycles per iteration
		false //override value
		};

LedPin* Led::createPin(uint8_t index, int8_t pin) {
	if ((index > (LED_DEFAULT_MAX_PINS - 1)) || _pins[index] != NULL)
		return NULL;
	LedPin* pPin = new LedPin(pin, this);
	pPin->_index = index;
	_pins[index] = pPin;
	return pPin;
}

LedPin* Led::getPin(int8_t pinNumber) {
	if (pinNumber > (LED_DEFAULT_MAX_PINS - 1))
		return NULL;
	return _pins[pinNumber];
}

void Led::setValue(int8_t pin, uint8_t value) {
	setValue(getPin(pin), value);
}

void Led::setValue(LedPin& pin, uint8_t value) {
	setValue(&pin, value);
}

void Led::setValue(LedPin* pin, uint8_t value) {
	if (pin == NULL)
		return;
	if (pin->isActive())
		pin->setActive(false);
	pin->setValue(value);
	//Write the pin state immediately
	writePinState(pin);
}

uint8_t Led::getValue(int8_t pin) {
	if (pin <= -1)
		return LED_INVALID_INDEX;
	return getValue(getPin(pin));
}

uint8_t Led::getValue(LedPin& pin) {
	return getValue(&pin);
}

uint8_t Led::getValue(LedPin* pin) {
	if (pin == NULL)
		return LED_INVALID_INDEX;
	return pin->getValue();
}

void Led::writePinState(LedFlow::LedPin *pin) {
	if (pin == NULL) {
		Serial.println("Pin or Pin Config is NULL");
		return;
	}

	LedPinConfig* cfg = pin->getConfig();

	if (cfg == NULL) {
		cfg = &LED_CONFIG_DEFAULT;
	}

	if (pin->isInvalidated()) {
		//Make sure we respect the min/max limits
		analogWrite(pin->getPin(),
				constrain(pin->getValue(), cfg->min, cfg->max));
	}
}

void Led::setPinActivityListener(LedPinActivityListener listener) {
	_pinActivityListener = listener;
}

LedPinActivityListener Led::getPinActivityListener(void) {
	return _pinActivityListener;
}

void Led::firePinActivityEvent(LedPin& pin, LedActivityEvent event) {
	if (_pinActivityListener != NULL) {
		_pinActivityListener(pin, event);
	}
	onPinEvent(pin, event);
}

void Led::start(LedActivity& activity) {
	stop();
	//Set the activity that will be shared by the led pins
	_activityManager.setActivity(&activity);
	_activityManager.repeatSet(0);

	//Make it available for reading again
	_activityManager.reset();

	//mark all pins active
	for (int i = 0; i < LED_DEFAULT_MAX_PINS; i++) {
		LedPin* pin = getPin(i);
		if (pin != NULL) {
			pin->setActive(true);
		}
	}
}

void Led::stop(void) {
	_activityManager.setActivity(NULL);
	for (int i = 0; i < LED_DEFAULT_MAX_PINS; i++) {
		LedPin* pin = getPin(i);
		if (pin != NULL) {
			if (pin->isActive()) {
				pin->setActive(false);
				firePinActivityEvent(*pin, LedActivityEvent::ABORTED);
			}
		}

	}
}

void Led::update(const uint32_t& now) {
	//Update the pins
	for (uint8_t i = 0; i < LED_DEFAULT_MAX_PINS; i++) {
		LedPin* pin = _pins[i];
		if ((pin != NULL && pin->getConfig() != NULL)
				&& (pin->getPin() > -1 && pin->isActive())) {
			updatePinState(pin, now);
			writePinState(pin);
		}
	}
}

void Led::onPinEvent(LedPin& pin, LedActivityEvent& event) {
	if (event == LedActivityEvent::STARTED) {
		LedPinConfig* cfg =
				(_activityManager.currentIndex(pin) == -1) ?
						_activityManager.next(pin) :
						_activityManager.current(pin);

		if (cfg == NULL)
			return;

		/*Serial.print(millis());
		 Serial.print(" :: ");
		 Serial.print("[PIN EVENT]: Pin = ");
		 Serial.print(pin.getPin());
		 Serial.print(", Activity = ");
		 Serial.print(_activityManager.currentIndex(pin));
		 Serial.print(", Interval = ");
		 Serial.print(cfg->interval);
		 Serial.println(", Event = STARTED");*/

		//Workaround for the timing issue in the delay
		if (!cfg->override)
			pin.setValid(true);

		if (!preProcessPin(pin, cfg, event))
			pin.setConfig(cfg);
	} else if (event == LedActivityEvent::COMPLETED
			|| event == LedActivityEvent::ABORTED) {

		/*Serial.print(millis());
		 Serial.print(" :: ");
		 Serial.print("[PIN EVENT]: Pin = ");
		 Serial.print(pin.getPin());
		 Serial.print(", Activity = ");
		 Serial.print(_activityManager.currentIndex(pin));
		 Serial.print(", Interval = ");
		 Serial.print(_activityManager.current(pin)->interval);
		 Serial.println(", Event = COMPLETED");*/

		if (_activityManager.hasNext(pin)) {
			pin.setConfig(_activityManager.next(pin));
			pin.setActive(true);
		} else {
			/* At this point, we are at the last activity step */
			//repeat forever
			if (_activityManager.getActivity().repeat() == 0) {
				_activityManager.reset(pin);
				pin.setActive(true);
			} else {
				//repeat limit reached
				if (_activityManager.repeatIncrement(pin)
						>= _activityManager.getActivity().repeat()) {
					_activityManager.repeatSet(pin, 0);
					_activityManager.reset(pin);
				}
				//repeat limit not yet reached
				else {
					_activityManager.reset(pin); //reset read index
					pin.setActive(true); //turn it active again
				}
			}
		}
	}
}

//This is where the magic happens
void Led::updatePinState(LedPin* pin, const uint32_t& now) {
	if (pin == NULL || pin->getConfig() == NULL) {
		return;
	}

	LedPinConfig& config = *pin->getConfig();

	//Initialize the offset value from the value of the  step direction
	int8_t offset = pin->getStepDirection();

	//Do not proceed if we haven't reached the desired interval
	if ((now - pin->_lastUpdated) < config.interval)
		return;

	//Update the pin's last update time
	pin->_lastUpdated = now;

	//If override is set, immediately write the
	//value to the pin and mark as complete
	if (config.override) {
		pin->setValue(config.startValue);
		pin->setValid(false);
		writePinState(pin);
		pin->complete();
		return;
	}

	if (pin->getStepDirection() == LedStepDirection::STEP_NONE
			&& pin->isActive()) {
		pin->complete();
		return;
	}

	//Do not proceed if we are not oscillating the value and
	//min/max limits have been reached
	if (!config.oscillate
			&& (((pin->getStepDirection() < LedStepDirection::STEP_NONE)
					&& pin->minLimitReached())
					|| ((pin->getStepDirection() > LedStepDirection::STEP_NONE)
							&& pin->maxLimitReached())) && config.repeat <= 0) {
		return;
	}

	//directly access member instead of getter to avoid triggering the invalidation state
	int32_t newValue = pin->_value;

	//Do not update the value if the direction is 0 (STEP_NONE)
	if (pin->getStepDirection() != LedStepDirection::STEP_NONE) {
		if (!config.override) {
			//#1 - Update Value
			if (config.smooth) {
				//Determine the offset value based on the direction specified
				if (pin->getStepDirection() > LedStepDirection::STEP_NONE)
					offset += config.stepUpOffset;
				else
					offset -= config.stepDownOffset;

				//update the new value with the offset
				newValue += offset;

				//make sure the newValue is within the limits
				newValue = constrain(newValue, config.min, config.max);
			} else {
				newValue =
						(pin->getStepDirection() > LedStepDirection::STEP_NONE) ?
								config.max : config.min;
			}

			//Update the pin value
			pin->setValue(newValue);

			//#2 - Automatically update direction if oscillation is enabled
			if (config.oscillate) {
				if (newValue == config.max)
					pin->setStepDirection(LedStepDirection::STEP_DOWN);
				else if (newValue == config.min)
					pin->setStepDirection(LedStepDirection::STEP_UP);
			}
		} //if not override

		//#3 - Determine repeatability
		if (config.repeat > 0) {
			//capture min/max value change events then increment the cycle counter
			if (newValue == config.max || newValue == config.min) {
				pin->_cycle++;
			}

			//Have we reached an iteration step? (cycle == cyclesPerIteration)
			if (pin->_cycle >= config.cyclesPerIteration) {
				//Check if we have reached the repeat limit
				if (++pin->_iteration >= config.repeat) {
					//Update direction to NONE to complete the led activity event
					//Make sure we re-update the step direction step after the call to reset
					//to prevent the activity from re-starting
					pin->setStepDirection(LedStepDirection::STEP_NONE);

					//Finally, reset the iteration and cycle counters
					pin->complete();
					return;
				}

				//Reset the pin state
				pin->reset();
				//Reset cycle counter
				pin->_cycle = 0;
			} // if repeat limit reached

		} //if repeat enabled

	} //if !LedStepDirection::STEP_NONE

}
} //namespace LedLib
