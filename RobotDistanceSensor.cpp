#include "RobotDistanceSensor.h"

const uint8_t F_POS = 90;
const uint8_t FL_POS = 140;
const uint8_t FR_POS = 40;

RobotDistanceSensor::RobotDistanceSensor(uint8_t in_servoPin, uint8_t in_triggerPin, uint8_t in_echoPin)
{
	triggerPin=in_triggerPin;
	echoPin=in_echoPin;

	pinMode(triggerPin, OUTPUT);
	pinMode(echoPin, INPUT);
	usServo.attach(in_servoPin);
}

int8_t RobotDistanceSensor::getDistance()
{
	digitalWrite(triggerPin, LOW);
	delayMicroseconds(2);
	digitalWrite(triggerPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(triggerPin, LOW);

	long duration = pulseIn(echoPin, HIGH);

	// for the infinity - return something big
	if(duration == 0) duration = 6000;

	return round(duration /29 / 2);
}

int8_t RobotDistanceSensor::getFrontDistance() {
	int8_t distance = lastFDistance;

	// can we read the distance instantly?
	if(usServo.read()==F_POS) {
		// read the distance from the sensor
		distance = getDistance();
	}

	return distance;
}

int8_t RobotDistanceSensor::getFrontLeftDistance() {
	int8_t distance = lastFLDistance;

	// can we read the distance instantly?
	if(usServo.read()==FL_POS) {
		// read the distance from the sensor
		distance = getDistance();
	}

	return distance;
}

int8_t RobotDistanceSensor::getFrontRightDistance() {
	int8_t distance = lastFRDistance;

	// can we read the distance instantly?
	if(usServo.read()==FR_POS) {
		// read the distance from the sensor
		distance = getDistance();
	}

	return distance;
}
