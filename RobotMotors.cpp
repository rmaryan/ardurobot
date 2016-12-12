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
	currentCommand = cmdFwd;

	leftMotor->setSpeed(speed);
	rightMotor->setSpeed(speed);
	leftMotor->run(FORWARD);
	rightMotor->run(FORWARD);

	scheduleTimedTask(duration);
}

void RobotMotors::driveBackward(uint8_t speed, uint16_t duration) {
	currentCommand = cmdFwd;

	leftMotor->setSpeed(speed);
	rightMotor->setSpeed(speed);
	leftMotor->run(BACKWARD);
	rightMotor->run(BACKWARD);

	scheduleTimedTask(duration);
}


void RobotMotors::turnRight(uint8_t speed, uint16_t duration) {
	currentCommand = cmdRight;

	leftMotor->setSpeed(speed);
	rightMotor->setSpeed(speed);
	leftMotor->run(FORWARD);
	rightMotor->run(BACKWARD);

	scheduleTimedTask(duration);
}

void RobotMotors::turnLeft(uint8_t speed, uint16_t duration) {
	currentCommand = cmdLeft;

	leftMotor->setSpeed(speed);
	rightMotor->setSpeed(speed);
	leftMotor->run(BACKWARD);
	rightMotor->run(FORWARD);

	scheduleTimedTask(duration);
}

void RobotMotors::fullStop() {
	currentCommand = cmdStop;

	leftMotor->run(RELEASE);
	rightMotor->run(RELEASE);
}

