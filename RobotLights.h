/*
 * RobotLights.h
 *
 *  Created on: 12 זמגע. 2016 נ.
 *      Author: snm
 */

#ifndef ROBOTLIGHTS_H_
#define ROBOTLIGHTS_H_

#include "TaskInterface.h"

/*
 * This class takes care about the robot LED lighting
 */
class RobotLights: public TaskInterface {
private:
	uint8_t frontPin;
	uint8_t rearPin;
public:
	/*
	 * frontPin - the pin connected to the front LED's
	 * rearPin - the pin connected to the rear LED's
	 */
	RobotLights(uint8_t in_frontPin, uint8_t in_rearPin);
	virtual ~RobotLights();
	virtual void processTask();

	/*
	 * Turn the LEDs on and off
	 */
	virtual void turnFrontLED(bool turnOn);
	virtual void turnRearLED(bool turnOn);
};

#endif /* ROBOTLIGHTS_H_ */
