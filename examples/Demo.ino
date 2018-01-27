#include <Led.h>

using namespace LedLib;

#define PIN_LED           6
#define PIN_LED_STATUS_R  2
#define PIN_LED_STATUS_G  3
#define PIN_LED_STATUS_B  4

//Define your custom led activities here
LedActivity activity_1 = LedActivity(11, 0)
                         .blink(true, 2, 5)
                         .delay(500)
                         .blink(true, 3, 0)
                         .delay(500)
                         .blink(false, 20, 50)
                         .delay(500)
                         .on()
                         .delay(1000)
                         .off()
                         .delay(1000);

//Instantiate the classes
LedManager ledManager = LedManager();
LedRgb ledRgb = LedRgb(PIN_LED_STATUS_R, PIN_LED_STATUS_G, PIN_LED_STATUS_B);
LedSingle ledSingle = LedSingle(PIN_LED);

void setup() {
  ledManager.add(ledSingle);
  ledManager.add(ledRgb);
    
  // put your setup code here, to run once:
  ledSingle.start(activity_1);
  ledRgb.start(activity_1);
}

void loop() {
  // put your main code here, to run repeatedly:
  ledManager.update();
}
