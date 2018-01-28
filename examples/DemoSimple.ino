#include <LedFlow.h>

using namespace LedFlow;

#define PIN_LED_STATUS_R  2
#define PIN_LED_STATUS_G  3
#define PIN_LED_STATUS_B  4
#define PIN_LED_SINGLE    5

/**
 * This is a demonstration on how to manually 
 */

LedRgb ledRgb = LedRgb(PIN_LED_STATUS_R, PIN_LED_STATUS_G, PIN_LED_STATUS_B);
LedSingle ledSingle = LedSingle(PIN_LED_SINGLE);

void setup() {

}

uint8_t value = 0;

void loop() {
	// Immediately write the value (using the macro RGB(red, green, blue)) to the gpio pin.
	// Note: Calling the update() method of the led instance is not necessary when using the setValue() method.
	if (red > 255)
		red = 0;
	ledRgb.setValue(RGB(value++, 0, 0));
	
	//Note: If you need to individually set the color without having to override the values of the other leds, do this: 
	//ledRgb.setValueRed(redValue);
	//ledRgb.setValueGreen(greenValue);
	//ledRgb.setValueBlue(blueValue);
	
	ledSingle.setValue(value);
}
