#include <LedFlow.h>

using namespace LedFlow;

#define PIN_LED_STATUS_R  2
#define PIN_LED_STATUS_G  3
#define PIN_LED_STATUS_B  4

LedRgb ledRgb = LedRgb(PIN_LED_STATUS_R, PIN_LED_STATUS_G, PIN_LED_STATUS_B);

void setup() {

}

uint8_t red = 0;

void loop() {
  // Immediately write the value (using the macro RGB(red, green, blue)) to the gpio pin.
  // As you can see, there is no need to call the update()
  // method when calling setValue().
  if (red > 255)
    red = 0;
  ledRgb.setValue(RGB(red++, 0, 0));
}
