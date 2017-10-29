/*
 * RobotLights.h
 *
 *  Created on: 12 זמגע. 2016 נ.
 *      Author: snm
 */

#ifndef ROBOTLIGHTS_H_
#define ROBOTLIGHTS_H_

#include "TaskInterface.h"

#include <Adafruit_NeoPixel.h>

/*
 * This class takes care about the robot LED lighting
 */
class RobotLights: public TaskInterface {
private:
	const uint16_t LED_DELAY = 1000;

	uint8_t frontPin;
	uint8_t rearPin;
	bool sideLedsActive = false;
	Adafruit_NeoPixel* leftPix = 0;
	Adafruit_NeoPixel* rightPix = 0;
public:
	/*
	 * frontPin - the pin connected to the front LED's
	 * rearPin - the pin connected to the rear LED's
	 */
	RobotLights(uint8_t in_frontPin, uint8_t in_rearPin, uint8_t in_leftPin, uint8_t in_rightPin);
	virtual ~RobotLights();
	virtual void processTask();

	/*
	 * Turn the LEDs on and off
	 */
	virtual void turnFrontLED(bool turnOn);
	virtual void turnRearLED(bool turnOn);
	virtual void turnSideLED(bool turnOn);
};

#endif /* ROBOTLIGHTS_H_ */
