/*
 * RobotMotors.cpp
 *
 */

#include "RobotMotors.h"
#include "Arduino.h"

RobotMotors::RobotMotors() {
	leftMotor = new AF_DCMotor(leftDriveID, MOTOR34_64KHZ);
	rightMotor = new AF_DCMotor(rightDriveID, MOTOR34_64KHZ);
}

RobotMotors::~RobotMotors() {
	fullStop();
	delete leftMotor;
	delete rightMotor;
}

void RobotMotors::processTask() {
	if(reachedDeadline()) {
		fullStop();
		taskDeadline = 0;
	}
}

void RobotMotors::driveForward(uint8_t speed, uint16_t duration) {
	runDrives(speed, duration, FORWARD, FORWARD);
}

void RobotMotors::driveBackward(uint8_t speed, uint16_t duration) {
	runDrives(speed, duration, BACKWARD, BACKWARD);
}


void RobotMotors::turnRight(uint8_t speed, uint16_t duration) {
	runDrives(speed, duration, RELEASE, BACKWARD);
}

void RobotMotors::turnLeft(uint8_t speed, uint16_t duration) {
	runDrives(speed, duration, BACKWARD, RELEASE);
}

void RobotMotors::runDrives(uint8_t speed, uint16_t duration,
		uint8_t leftDirection, uint8_t rightDirection) {
	leftMotor->setSpeed(speed);
	rightMotor->setSpeed(speed);
	leftMotor->run(leftDirection);
	rightMotor->run(rightDirection);

	scheduleTimedTask(duration);
}

void RobotMotors::fullStop() {
	leftMotor->run(RELEASE);
	rightMotor->run(RELEASE);
}

void RobotMotors::setMotorSpeed(uint16_t leftSpeed, uint16_t rightSpeed, uint16_t duration) {
	leftSpeed = constrain(leftSpeed, 0, 511);
	rightSpeed = constrain(rightSpeed, 0, 511);

	if(leftSpeed == 255) {
		leftMotor->run(RELEASE);
	} else
		if(leftSpeed > 255) {
			leftMotor->setSpeed(leftSpeed-255);
			leftMotor->run(FORWARD);
		} else {
			leftMotor->setSpeed(255 - leftSpeed);
			leftMotor->run(BACKWARD);
		}

	if(rightSpeed == 255) {
		rightMotor->run(RELEASE);
	} else
		if(rightSpeed > 255) {
			rightMotor->setSpeed(rightSpeed-255);
			rightMotor->run(FORWARD);
		} else {
			rightMotor->setSpeed(255 - rightSpeed);
			rightMotor->run(BACKWARD);
		}

	scheduleTimedTask(duration);
}
