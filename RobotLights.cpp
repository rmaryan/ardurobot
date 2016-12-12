/*
 * RobotLights.cpp
 *
 *  Created on: 12 זמגע. 2016 נ.
 *      Author: snm
 */

#include "RobotLights.h"

RobotLights::RobotLights(uint8_t in_dataPin, uint8_t in_syncPin, uint8_t in_latchPin) {
	dataPin = in_dataPin;
	syncPin = in_syncPin;
	latchPin = in_latchPin;

	//TODO FIXME - temporary blinking mode
	latchPin = 0;
	scheduleTimedTask(2000);
}

RobotLights::~RobotLights() {

}

void RobotLights::processTask() {
	if(reachedDeadline()) {
		if(latchPin == 0) {
			// turn on the leds
			digitalWrite(dataPin, HIGH);
			digitalWrite(syncPin, HIGH);
			latchPin = 1;
		} else {
			// turn off the leds
			digitalWrite(dataPin, LOW);
			digitalWrite(syncPin, LOW);
			latchPin = 0;
		}
		scheduleTimedTask(2000);
	}

}
