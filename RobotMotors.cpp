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
	runDrives(speed, duration, FORWARD, FORWARD);
}

void RobotMotors::driveBackward(uint8_t speed, uint16_t duration) {
	currentCommand = cmdFwd;
	runDrives(speed, duration, BACKWARD, BACKWARD);
}


void RobotMotors::turnRight(uint8_t speed, uint16_t duration) {
	currentCommand = cmdRight;
	runDrives(speed, duration, FORWARD, BACKWARD);
}

void RobotMotors::turnLeft(uint8_t speed, uint16_t duration) {
	currentCommand = cmdLeft;
	runDrives(speed, duration, BACKWARD, FORWARD);
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
	currentCommand = cmdStop;

	leftMotor->run(RELEASE);
	rightMotor->run(RELEASE);
}

