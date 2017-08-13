/*
 * RobotLights.cpp
 *
 *  Created on: 12 זמגע. 2016 נ.
 *      Author: snm
 */

#include "RobotLights.h"

RobotLights::RobotLights(uint8_t in_frontPin, uint8_t in_rearPin) {
	frontPin = in_frontPin;
	rearPin = in_rearPin;

	pinMode(frontPin, OUTPUT);
	pinMode(rearPin, OUTPUT);

	scheduleTimedTask(2000);
}

RobotLights::~RobotLights() {

}

void RobotLights::turnFrontLED(bool turnOn) {
	digitalWrite(frontPin, turnOn);
}

void RobotLights::turnRearLED(bool turnOn) {
	digitalWrite(rearPin, turnOn);
}

void RobotLights::processTask() {
// No special scenarios used yet
//	if(reachedDeadline()) {
//		if(latchPin == 0) {
//			// turn on the leds
//			digitalWrite(dataPin, HIGH);
//			digitalWrite(syncPin, HIGH);
//			latchPin = 1;
//		} else {
//			// turn off the leds
//			digitalWrite(dataPin, LOW);
//			digitalWrite(syncPin, LOW);
//			latchPin = 0;
//		}
//		scheduleTimedTask(2000);
//	}

}
