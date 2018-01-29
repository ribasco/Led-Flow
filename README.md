![](resources/logo.png) 

# Introduction

If you need to control a series of led activities/patterns in a non-blocking fashion then this library is for you. 

# Features
- Non-blocking
- Fluent API Interface
- Define a chain of led activity configurations that you can assign, reuse and playback for any led instance.
- More flexibility in controlling the light patterns for complex activities

# Installation

#### Arduino

1. Browse to the arduino library/sketchbook location
2. Navigate to the libraries folder
3. Clone the repository into the libraries folder 

	`git clone https://github.com/ribasco/Led-Flow.git`

4. Restart the Arduino IDE (If open)
5. In your sketch, include the library by typing

```c++
#include <LedFlow.h>

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
```
or simply via the Arduino Menu

	Sketch -> Include Library -> Led Flow
	
# Usage

#### Basic Usage

Please note that using the setValue* methods are the same as calling analogWrite(value).

1. Turning on/off a Standard LED

```c++
#include <LedFlow.h>

#define PIN_LED           6

LedSingle ledSingle = LedSingle(PIN_LED); 

void setup() {
        //Turn On: Set to the maximum brightness (255)
	ledSingle.setValue(255);
	
	//Turn off
	//ledSingle.setValue(0);
}

void loop() {
}
```

2. Turning on/off an RGB LED

```c++
#include <LedFlow.h>

#define PIN_LED_R 	2
#define PIN_LED_G 	3
#define PIN_LED_B 	4

LedRgb ledRgb = LedRgb(PIN_LED_R, PIN_LED_G, PIN_LED_B); 

void setup() {
        //Only turn on color red to it's maximum brightness
	ledRgb.setValue(RGB(255, 0, 0));
	 
	//The following code below individually sets the color value of each RGB Pin. Using this method will not override the values of the other pins. 
	//ledRgb.setValueRed(0);
	//ledRgb.setValueGreen(255);
	//ledRgb.setValueBlue(255);
}

void loop() {


}
```

#### Advanced Usage

under construction...
