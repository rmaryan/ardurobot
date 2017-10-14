/*
 * RobotLights.cpp
 *
 *  Created on: 12 זמגע. 2016 נ.
 *      Author: snm
 */

#include "RobotLights.h"

RobotLights::RobotLights(uint8_t in_frontPin, uint8_t in_rearPin, uint8_t in_leftPin, uint8_t in_rightPin) {
	frontPin = in_frontPin;
	rearPin = in_rearPin;

	pinMode(frontPin, OUTPUT);
	pinMode(rearPin, OUTPUT);

	// Initialize the NeoPixel objects
	leftPix = new Adafruit_NeoPixel(3, in_leftPin, NEO_RGB + NEO_KHZ800);
	rightPix = new Adafruit_NeoPixel(3, in_rightPin, NEO_RGB + NEO_KHZ800);

	leftPix->begin();
	rightPix->begin();

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

void RobotLights::turnSideLED(bool turnOn) {
	sideLedsActive = turnOn;
}

void RobotLights::processTask() {
	if(reachedDeadline()) {
		// if the side lights turned on - light the LED's in random colors
		if(sideLedsActive) {
			leftPix->setPixelColor(0, random(0,255), random(0,255), random(0,255));
			leftPix->setPixelColor(1, random(0,255), random(0,255), random(0,255));
			leftPix->setPixelColor(2, random(0,255), random(0,255), random(0,255));
			leftPix->show();
			rightPix->setPixelColor(0, random(0,255), random(0,255), random(0,255));
			rightPix->setPixelColor(1, random(0,255), random(0,255), random(0,255));
			rightPix->setPixelColor(2, random(0,255), random(0,255), random(0,255));
			rightPix->show();
		} else {
			// turn off the LEDS
			leftPix->clear();
			leftPix->show();
			rightPix->clear();
			rightPix->show();
		}

		scheduleTimedTask(LED_DELAY);
	}
}
