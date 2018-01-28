#ifndef LEDFLOW_H_
#define LEDFLOW_H_

namespace LedFlow {

#include <Arduino.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#define LED_DEBUG_ENABLED         false
#define LED_PWM_MIN               0
#define LED_PWM_MAX               255

#define LED_MAX_STEPUP            floor(LED_PWM_MAX / 2) - 1
#define LED_MAX_STEPDN            floor(LED_PWM_MAX / 2) - 1
#define LED_INVALID_INDEX         -1
#define LED_DEFAULT_COLOR         RGB(0, 255, 0)
#define LED_DEFAULT_MAX_PINS      4
#define LED_DEFAULT_STEPUP        0
#define LED_DEFAULT_STEPDN        0
#define LED_DEFAULT_OSCILLATE     true
#define LED_DEFAULT_INTERVAL      1
#define LED_DEFAULT_SMOOTH        true
#define LED_DEFAULT_STEPDIR       LedStepDirection::STEP_NONE
#define LED_DEFAULT_REPEAT        0
#define LED_DEFAULT_CPI           2
#define LED_DEFAULT_VALUE         127

#define LEDM_MAX_LEDS             10
#define LEDM_DEFAULT_INTERVAL     1

//macro definitions for converting byte(s) to unsigned 32-bit integer (long)
#define RGBW(R, G, B, W)  ((uint32_t) R << 24 | (uint32_t) G << 16 | (uint32_t) B << 8 | (uint32_t) W)
#define RGB(R, G, B)      (RGBW(R, G, B, 0))
#define RED(R)            (RGBW(R, 0, 0, 0))
#define GREEN(G)          (RGBW(0, G, 0, 0))
#define BLUE(G)           (RGBW(0, 0, B, 0))
#define WHITE(G)          (RGBW(0, 0, 0, W))

//macro definitions for converting unsigned 32-bit long to byte (RGB)
#define R_BYTE(value) ((value >> 24) & 0xFF)
#define G_BYTE(value) ((value >> 16) & 0xFF)
#define B_BYTE(value) ((value >> 8) & 0xFF)
#define W_BYTE(value) (value & 0xFF)

//Forward declarations
class Led;
class LedPin;
class LedSingle;
class LedRgb;
struct LedPinConfig;

extern LedPinConfig LED_CONFIG_DEFAULT;
extern LedPinConfig LED_CONFIG_DELAY;
extern LedPinConfig LED_CONFIG_FADE_IN;
extern LedPinConfig LED_CONFIG_FADE_OUT;
extern LedPinConfig LED_CONFIG_ON;
extern LedPinConfig LED_CONFIG_OFF;
extern LedPinConfig LED_CONFIG_BLINK;
extern LedPinConfig LED_CONFIG_BLINK_SMOOTH;

/**
 * An enum containing the Step Directions for an Led Pin
 *
 * @author Rafael Ibasco
 */
enum LedStepDirection {
	STEP_UP = 1, STEP_NONE = 0, STEP_DOWN = -1
};

enum LedActivityEvent {
	STARTED, COMPLETED, ABORTED, CONFIG_CHANGED, INVALIDATED
};

//Callback definitions
typedef void (*LedPinActivityListener)(LedPin&, LedActivityEvent);

struct LedPinConfig {
	uint8_t startValue;
	LedStepDirection startDirection;
	uint8_t stepUpOffset;
	int8_t stepDownOffset;
	uint8_t min;
	uint8_t max;bool oscillate;bool smooth;
	uint32_t interval;
	uint16_t repeat;
	uint8_t cyclesPerIteration;bool override;
	uint32_t color;bool equals(const LedPinConfig& rhs) {
		return startValue == rhs.startValue
				&& startDirection == rhs.startDirection
				&& stepUpOffset == rhs.stepUpOffset
				&& stepDownOffset == rhs.stepDownOffset && min == rhs.min
				&& max == rhs.max && oscillate == rhs.oscillate
				&& smooth == rhs.smooth && interval == rhs.interval
				&& repeat == rhs.repeat
				&& cyclesPerIteration == rhs.cyclesPerIteration
				&& override == rhs.override && color == rhs.color;
	}

	bool operator ==(const LedPinConfig& rhs) {
		return equals(rhs);
	}

	bool operator !=(const LedPinConfig& rhs) {
		return !equals(rhs);
	}
};

/**
 * Contains all the state data associated with an LED Pin
 *
 * @author Rafael Ibasco
 */
class LedPin {
	friend class Led;
public:
	LedPin(void) :
			LedPin(-1, NULL) {
	}
	LedPin(int8_t pin, Led* led) {
		_pin = pin;
		_invalidated = true;
		_lastUpdated = 0;
		_iteration = 0;
		_cycle = 0;
		_active = false;
		_value = LED_CONFIG_DEFAULT.startValue;
		_direction = LED_CONFIG_DEFAULT.startDirection;
		//_config = LED_CONFIG_DEFAULT;
		_config = NULL;
		_led = led;
	}

	bool operator ==(const LedPin& rhs) {
		return _pin == rhs._pin;
	}

	bool operator !=(const LedPin& rhs) {
		return _pin != rhs._pin;
	}

	uint8_t index(void);

	/**
	 * Set the GPIO pin number for this LED pin
	 *
	 * @param pin An unsigned byte
	 */
	void setPin(int8_t pin);

	/**
	 * @return Returns the GPIO pin number associated with the LED pin
	 */
	int8_t getPin(void);

	/**
	 * Set the PWM value of the LED pin (Range: 0 to 255)
	 *
	 * @param value An unsigned byte
	 */
	void setValue(uint8_t value);

	/**
	 * Returns the PWM value set for this LED pin (Range: 0 to 255)
	 *
	 * @return An unsigned byte
	 */
	uint8_t getValue(void);

	void setStepDirection(LedStepDirection direction);

	LedStepDirection getStepDirection(void);

	uint32_t getIteration(void);

	/**
	 * Flag to indicate if the PWM value has been modified from its previous state.
	 *
	 * Other notes:
	 *  - A call to #setValue() will invalidate the pin state if there are
	 *    any changes made since the last update.
	 *  - A call to #getValue() will validate the pin state
	 *
	 * @return  True if the value has been modified
	 */
	bool isInvalidated(void);

	void setConfig(LedPinConfig* config);

	LedPinConfig* getConfig(void);

	/**
	 * Resets the pin state back to their starting values
	 */
	void reset(void);

	/**
	 * Mark the pin as completed
	 */
	void complete(void);

	bool isActive(void);

	void setActive(bool active);

	void setLed(Led* led);

	void setValid(bool valid) {
		_invalidated = !valid;
	}

	Led* getLed(void);
private:
	Led* _led;

	LedPinConfig* _config;

	/**
	 * Keeping track when this led was last updated (in milliseconds)
	 */
	uint32_t _lastUpdated;

	/**
	 * The current index of the LedPin instance in the array
	 */
	uint8_t _index = -1;

	/**
	 * The GPIO Pin number of the microcontroller
	 */
	int8_t _pin = -1;

	/**
	 * The value of the pin (Minimum = 0, Maximum = 255)
	 */
	uint8_t _value;

	/**
	 * The flow of direction (increment/decrement) of the PWM value
	 *
	 * @param _direction A signed byte (e.g. 0 = No increment/decrement, 1 = increment by 1, -1 = decrement by 1)
	 */
	LedStepDirection _direction;

	/**
	 * True when the value has been modified from its previous state
	 */
	bool _invalidated;

	bool _active;

	uint32_t _iteration;

	uint8_t _cycle;

	bool minLimitReached(void);

	bool minLimitReached(uint8_t value);

	bool maxLimitReached(void);

	bool maxLimitReached(uint8_t value);
};

class LedActivity {
	friend class Led;
	friend class LedSingle;
	friend class LedRgb;
	friend class LedActivityManager;
public:
	LedActivity(void) :
			LedActivity(1) {
	}
	LedActivity(uint8_t numOfSteps) :
			LedActivity(numOfSteps, 0) {
	}
	LedActivity(uint8_t numOfSteps, int8_t repeat) :
			LedActivity(NULL, numOfSteps, repeat) {
	}
	LedActivity(Led* led, uint8_t numOfSteps, int8_t repeat) {
		_repeat = repeat;
		_count = numOfSteps;
		_writeIndex = 0;
		_led = led;
		_steps = new LedPinConfig[_count];
	}
	virtual ~LedActivity() {
	}
	uint16_t& repeat(void);
	LedActivity& repeat(uint16_t repeat);
	LedActivity& delay(uint32_t interval);
	LedActivity& on(void);
	LedActivity& on(uint8_t brightnessPct);
	LedActivity& on(uint8_t brightnessPct, uint32_t color);
	LedActivity& off(void);
	LedActivity& blink(bool smooth, uint16_t repeat, uint32_t interval);
	LedActivity& blink(bool smooth, uint16_t repeat, uint32_t interval,
			uint32_t color);
	LedActivity& fade(uint32_t from, uint32_t to, uint16_t repeat,
			uint32_t interval);
	LedActivity& fade(uint32_t from, uint32_t to, uint16_t repeat,
			uint32_t interval, uint32_t color);
	LedActivity& custom(LedPinConfig& config);
protected:
	Led* _led;
	LedActivity* addStep(LedPinConfig step);
private:
	LedPinConfig* _steps;
	uint16_t _repeat;
	int16_t _count;
	int8_t _writeIndex;
	int16_t& count(void);
	LedPinConfig* step(int8_t index);
};

class LedActivityManager {
	friend class Led;
	friend class LedSingle;
	friend class LedRgb;
public:
	LedActivityManager(void) :
			LedActivityManager(NULL) {
	}
	LedActivityManager(Led* led) {
		_activity = NULL;
		_led = led;
	}
	virtual ~LedActivityManager() {
	}
	void setActivity(LedActivity* activity);
	LedActivity& getActivity(void);

	void reset(void);
	void reset(LedPin& pin);
	LedPinConfig* current(LedPin& pin);
	LedPinConfig* next(LedPin& pin);
	int8_t currentIndex(LedPin& pin);bool hasNext(LedPin& pin);
	uint16_t repeat(LedPin& pin);
	uint16_t repeatIncrement(LedPin& pin);
	void repeatSet(uint16_t value);
	void repeatSet(LedPin& pin, uint16_t value);
private:
	LedActivity* _activity;
	int8_t _readIndex[LED_DEFAULT_MAX_PINS] = { -1 };
	uint16_t _repeatCtr[LED_DEFAULT_MAX_PINS] = { 0 };
	Led* _led;bool isValidPin(LedPin& pin) {
		if ((pin.index() >= 0) && (pin.index() <= LED_DEFAULT_MAX_PINS))
			return true;
		return false;
	}
};

/**
 * Static Counter which serves as a
 * unique identifier for all Led instances.
 *
 * DO NOT MODIFY THIS VARIABLE
 */
static int _idctr;

class Led {
	friend class LedPin;
	friend class LedManager;
public:
	/**
	 * A variable for identifying unique led instances
	 */
	const int _id = _idctr++;

	/**
	 * Constructor
	 */
	Led(void) {
		_pinActivityListener = NULL;
		_activityManager = LedActivityManager(this);
	}

	virtual ~Led() {
	}

	virtual void setValue(uint32_t value) {
	}
	void setValue(int8_t pin, uint8_t value);
	void setValue(LedPin& pin, uint8_t value);
	void setValue(LedPin* pin, uint8_t value);

	virtual uint32_t getValue(void) = 0;
	uint8_t getValue(int8_t pin);
	uint8_t getValue(LedPin& pin);
	uint8_t getValue(LedPin* pin);

	virtual void start(LedActivity& activity);
	virtual void stop(void);
	virtual void update() {
		update(millis());
	}
	virtual void update(const uint32_t& now);

	void setPinConfig(LedPin* pin, LedPinConfig& config);
	LedPinConfig& getPinConfig(LedPin* pin);

	void setPinActivityListener(LedPinActivityListener listener);
	LedPinActivityListener getPinActivityListener(void);
protected:
	LedActivityManager _activityManager;

	/**
	 * Retrieves a pointer to a LedPin based on the pin number specified. Returns NULL if no pin has been found
	 */
	LedPin* getPin(int8_t pinNumber);

	/**
	 * Writes the pin state to the GPIO Pins
	 */
	void writePinState(LedPin* pin);

	/**
	 * This is the method that is responsible for incrementing or decrementing
	 * the led's pin value (pwm). This is repeatedly called from within the loop.
	 */
	void updatePinState(LedPin* pin, const uint32_t& now);

	/**
	 * Convenience method to create a pin configuration instance.
	 * The led pins are stored inside this class that will be
	 * utilized by the derived classes.
	 */
	LedPin* createPin(uint8_t index, int8_t pin);

	virtual void onPinEvent(LedPin& pin, LedActivityEvent& event);

	virtual bool preProcessPin(LedPin& pin, LedPinConfig* config,
			LedActivityEvent& event) {
		pin.setConfig(config);
		return true;
	}
private:
	LedPin* _pins[LED_DEFAULT_MAX_PINS];

	LedPinActivityListener _pinActivityListener;

	void firePinActivityEvent(LedPin& pin, LedActivityEvent event);
};
//class Led

class LedSingle: public Led {
public:
	static const uint8_t IDX_LED = 0;

	LedSingle(int8_t pin);

	LedPin& getPin(void);
	void setValue(uint32_t value); //override base
	uint32_t getValue(void); //override base
private:
	LedPin* _pin;
};

class LedRgb: public Led {
public:
	static const uint8_t IDX_LED_RED = 0;
	static const uint8_t IDX_LED_GREEN = 1;
	static const uint8_t IDX_LED_BLUE = 2;

	LedRgb(int8_t redPin, int8_t greenPin, int8_t bluePin);

	/**
	 * Sets the values of all RGB pins (Overrides the base method)
	 *
	 * @param value An unsigned byte ranging from 0 to 255
	 */
	void setValue(uint32_t value);
	void setValueRed(uint8_t value);
	void setValueGreen(uint8_t value);
	void setValueBlue(uint8_t value);

	//@Override
	uint32_t getValue(void);
	uint8_t getValueRed(void);
	uint8_t getValueGreen(void);
	uint8_t getValueBlue(void);

	LedPin& getRedPin(void);
	LedPin& getGreenPin(void);
	LedPin& getBluePin(void);
protected:
	bool preProcessPin(LedPin& pin, LedPinConfig* config,
			LedActivityEvent& event);
private:
	LedPin* _redPin;
	LedPin* _greenPin;
	LedPin* _bluePin;
	LedPinConfig _tmp[3];
};

class LedManager {
public:
	LedManager(void) :
			_interval(LEDM_DEFAULT_INTERVAL), _previousMillis(0) {
	}
	int8_t add(Led& led);
	int8_t remove(Led& led);
	int8_t count(void);
	void setValue(uint32_t value);
	void setUpdateInterval(uint32_t interval);
	void update(void);
private:
	uint32_t _interval;
	uint32_t _previousMillis;
	Led* _leds[LEDM_MAX_LEDS];
	int findLedIndex(Led& led);
	int findFreeIndex(void);
	void updateAllLeds(const uint32_t& now);
};
} //end of namespace

#endif
