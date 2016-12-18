#include "RobotDistanceSensor.h"

RobotDistanceSensor::RobotDistanceSensor(uint8_t in_servoPin, uint8_t in_triggerPin, uint8_t in_echoPin)
{
	triggerPin=in_triggerPin;
	echoPin=in_echoPin;

	pinMode(triggerPin, OUTPUT);
	pinMode(echoPin, INPUT);
	usServo.attach(in_servoPin);
	// make sure we are ready for the front distance measurements
	Serial.write("FRONT 1\n");
	usServo.write(F_POS);
}

RobotDistanceSensor::~RobotDistanceSensor() {
	;
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
	if(duration == 0)
		return 100;
	else
		return round(duration /29 / 2);
}

int8_t RobotDistanceSensor::getFrontDistance() {
	int8_t distance = lastFDistance;

	// can we read the distance instantly?
	// the servo myust point to front and be stable
	if((usServo.read()==F_POS) &&
			((dsState == dsIdle)||(dsState == dsResultsReady))) {
		// read the distance from the sensor
		distance = getDistance();
	}
	return distance;
}

int8_t RobotDistanceSensor::getFrontLeftDistance() {
	int8_t distance = lastFLDistance;

	// can we read the distance instantly?
	if((usServo.read()==FL_POS) &&
			((dsState == dsIdle)||(dsState == dsResultsReady))) {
		// read the distance from the sensor
		distance = getDistance();
	}

	return distance;
}

int8_t RobotDistanceSensor::getFrontRightDistance() {
	int8_t distance = lastFRDistance;

	// can we read the distance instantly?
	if((usServo.read()==FR_POS) &&
			((dsState == dsIdle)||(dsState == dsResultsReady))) {
		// read the distance from the sensor
		distance = getDistance();
	}

	return distance;
}

void RobotDistanceSensor::querySideDistances() {
	// start measuring from the front-right
	dsState = dsMeasuringFR;

	// wait till servo will finish turning
	// it will take from SERVO_DELAY to 2*SERVO_DELAY
	uint16_t servoDelay = (usServo.read()>F_POS) ? 2*SERVO_DELAY : SERVO_DELAY;

	Serial.write("FRONT-RIGHT\n");
	usServo.write(FR_POS);

	scheduleTimedTask(servoDelay);
}


void RobotDistanceSensor::processTask() {
	if (reachedDeadline()) {
		switch (dsState) {
		case dsIdle:
			break; //do nothing
		case dsMeasuringFR:
			// the servo finished turning
			// measure the distance and turn left
			lastFRDistance = getDistance();
			Serial.write("FRONT LEFT\n");
			usServo.write(FL_POS);

			dsState = dsMeasuringFL;
			scheduleTimedTask(SERVO_DELAY);
			break;
		case dsMeasuringFL:
			// the servo finished turning
			// measure the distance and turn forward
			lastFLDistance = getDistance();
			Serial.write("FRONT 2\n");
			usServo.write(F_POS);

			dsState = dsMeasuringFF;
			scheduleTimedTask(SERVO_DELAY);
			break;
		case dsMeasuringFF:
			// the servo finished turning
			// measure the distance and wait for the validity period
			lastFDistance = getDistance();

			dsState = dsResultsReady;
			scheduleTimedTask(VALIDITY_PERIOD);
			break;
		case dsResultsReady:
			// validity period ended
			// drop the distance values
			lastFDistance = -1;
			lastFLDistance = -1;
			lastFRDistance = -1;

			dsState = dsIdle;
			break;
		}
	}
}
