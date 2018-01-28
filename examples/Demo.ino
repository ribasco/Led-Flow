#include <LedFlow.h>

#define PIN_LED           6
#define PIN_LED_STATUS_R  2
#define PIN_LED_STATUS_G  3
#define PIN_LED_STATUS_B  4

//Step #1: Define your custom led activities here
LedActivity crazy_blinking_and_stuff = LedActivity(10, 0) //Args: (Num of Steps, Repeat)
.blink(true, 2, 5)    //step 1:  Blink smoothly (PWM fade in/out) 2 times with 5ms interval 
.delay(500)           //step 2:  Delay for 500 ms before proceeding to the next step
.blink(true, 3, 0)    //step 3:  Blink smoothly (PWM fade in/out) 3 times with no interval (will blink as fast as possible)
.delay(500)           //step 4:  Delay for 500 ms before proceeding to the next step
.blink(false, 20, 50) //step 5:  Blink on and off (No PWM) 20 times with 50 ms interval
.delay(500)           //step 6:  Delay for 500 ms before proceeding to the next step 
.on()                 //step 7:  Turn the led on (Highest value set to 255)
.delay(1000)          //step 8:  Delay for 1 second before proceeding to the next step 
.off()                //step 9:  Turn off the led (Lowest value set to 0)
.delay(1000);         //step 10: Delay for 1 second before proceeding to the next step 

//Step #2: Instantiate the classes
LedManager ledManager = LedManager(); //this is optional
LedRgb ledRgb = LedRgb(PIN_LED_STATUS_R, PIN_LED_STATUS_G, PIN_LED_STATUS_B); //rgb led
LedSingle ledSingle = LedSingle(PIN_LED); //single led

void setup() {
	//Step #3 (Optional): Add to ledManager instance (Note: you can call update() directly from the Led instance if you are only dealing with one or two leds)
	ledManager.add(ledSingle);
	ledManager.add(ledRgb);

	//Step #4: Start the pre-defined activity to both leds
	ledSingle.start(crazy_blinking_and_stuff);
	ledRgb.start(crazy_blinking_and_stuff);
}

void loop() {
	// Step #5:
	// Since we are using the LedManager, call the update method.
	// Warning: Adding a delay inside the loop will cause undesirable effects
	//to the leds
	ledManager.update();
}
