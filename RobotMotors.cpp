/*
 * RobotMotors.cpp
 *
 */

#include "RobotMotors.h"
#include "Arduino.h"

RobotMotors::RobotMotors(uint8_t in_leftDrivePin,
		uint8_t in_rightDrivePin) {
	leftDrivePin = in_leftDrivePin;
	rightDrivePin = in_rightDrivePin;
	pinMode(leftDrivePin, OUTPUT);
	pinMode(rightDrivePin, OUTPUT);
}

RobotMotors::~RobotMotors() {
	fullStop();
}

void RobotMotors::processTask() {
	if(reachedDeadline()) {
		fullStop();
		taskDeadline = 0;
	}
}

void RobotMotors::driveForward(uint8_t speed, uint16_t duration) {
	currentCommand = cmdFwd;
	analogWrite(leftDrivePin, speed);
	analogWrite(rightDrivePin, speed);
	scheduleTimedTask(duration);
}

void RobotMotors::turnRight(uint8_t speed, uint16_t duration) {
	currentCommand = cmdRight;
	analogWrite(leftDrivePin, speed);
	analogWrite(rightDrivePin, 0);
	scheduleTimedTask(duration);
}

void RobotMotors::turnLeft(uint8_t speed, uint16_t duration) {
	currentCommand = cmdLeft;
	analogWrite(leftDrivePin, 0);
	analogWrite(rightDrivePin, speed);
	scheduleTimedTask(duration);
}

void RobotMotors::fullStop() {
	currentCommand = cmdStop;
	analogWrite(leftDrivePin, 0);
	analogWrite(rightDrivePin, 0);
}

