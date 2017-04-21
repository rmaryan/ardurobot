#include "RobotDistanceSensor.h"

RobotDistanceSensor::RobotDistanceSensor(uint8_t in_servoPin, uint8_t in_triggerPin, uint8_t in_echoPin,
		uint8_t in_abyssLeftPin, uint8_t in_abyssRightPin, uint8_t in_IRFrontLeftPin, uint8_t in_IRFrontRightPin,
		RobotConnector* in_robotConnector)
{
	triggerPin=in_triggerPin;
	echoPin=in_echoPin;

	pinMode(triggerPin, OUTPUT);
	pinMode(echoPin, INPUT);
	usServo.attach(in_servoPin);
	// make sure we are ready for the front distance measurements
	usServo.write(F_POS);

	abyssLeftPin = in_abyssLeftPin;
	abyssRightPin = in_abyssRightPin;
	irFrontLeftPin = in_IRFrontLeftPin;
	irFrontRightPin = in_IRFrontRightPin;

	pinMode(abyssLeftPin, INPUT);
	pinMode(abyssRightPin, INPUT);
	pinMode(irFrontLeftPin, INPUT);
	pinMode(irFrontRightPin, INPUT);

	robotConnector = in_robotConnector;
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
	if((duration == 0)||(duration>2900))
		return 100;
	else
		return round(duration /29 / 2);
}

int8_t RobotDistanceSensor::getFrontDistance() {
	long currentTimestamp = millis();

	if((lastFDistanceTimeStamp + FRONT_MEASURE_VALIDITY)<currentTimestamp) {

		// can we read the distance instantly?
		// the servo must point to front and be stable
		if((usServo.read()==F_POS) &&
				(dsState == dsIdle)) {
			// read the distance from the sensor
			lastFDistance = getDistance();
			lastFDistanceTimeStamp = currentTimestamp;
		} else  {
			lastFDistance = -1;
		}
	}
	return lastFDistance;
}

int8_t RobotDistanceSensor::getLastFrontLeftDistance() {
	return lastFLDistance;
}

int8_t RobotDistanceSensor::getLastFrontRightDistance() {
	return lastFRDistance;
}

void RobotDistanceSensor::querySideDistances(bool in_sendDistances) {
	// start measuring from the front-right
	dsState = dsMeasuringFR;
	sendDistances = in_sendDistances;

	lastFLDistance = -1;
	lastFRDistance = -1;

	usServo.write(FR_POS);

	// wait till servo will finish turning
	scheduleTimedTask(SERVO_DELAY);
}

void RobotDistanceSensor::processTask() {
	if(dsState != dsIdle) {
		if (reachedDeadline()) {
			switch (dsState) {
			case dsIdle:
				break;
			case dsMeasuringFR:
				// the servo finished turning
				// measure the distance and turn left
				lastFRDistance = getDistance();
				usServo.write(FL_POS);

				dsState = dsMeasuringFL;
				scheduleTimedTask(SERVO_DELAY);
				break;
			case dsMeasuringFL:
				// the servo finished turning
				// measure the distance and turn forward
				lastFLDistance = getDistance();
				usServo.write(F_POS);

				dsState = dsMeasuringFF;
				scheduleTimedTask(SERVO_DELAY);
				break;
			case dsMeasuringFF:
				// the servo finished turning
				// measure the distance and set the timestamp
				lastFDistance = getDistance();
				lastFDistanceTimeStamp = millis();

				// should we send the distance measures to the RC client?
				if(sendDistances) {
					sendFrontDistance(lastFDistance, 'F');
					sendFrontDistance(lastFLDistance, 'L');
					sendFrontDistance(lastFRDistance, 'R');
					sendDistances = false;
				}
				dsState = dsIdle;
				break;
			}
		}
	}
}

FrontObstacleStatus RobotDistanceSensor::isObstacleInFront() {
	// the front IR sensors are the fastest and the easiest to access
	if(getFrontLeftIRDetected() || getFrontRightIRDetected()) {
		return foOBSTACLE;
	} else {
		int8_t frontUSDistance = getFrontDistance();
		if(frontUSDistance == -1) {
			return foUNKNOWN;
		} else {
			if(frontUSDistance < MIN_DISTANCE) {
				return foOBSTACLE;
			}
		}
	}
	return foFREE;
}

ObstacleDirections RobotDistanceSensor::getObstacleDirection() {
	// check the abyss detectors first
	bool leftFlag = getFrontLeftAbyssDetected();
	bool rightFlag = getFrontRightAbyssDetected();

	if(leftFlag) {
		if(rightFlag) {
			return odBOTH;
		} else {
			return odLEFT;
		}
	} else {
		if(rightFlag) {
			return odRIGHT;
		}
	}

	// check IR sensors
	leftFlag = getFrontLeftIRDetected();
	rightFlag = getFrontRightIRDetected();

	if(leftFlag) {
		if(rightFlag) {
			return odBOTH;
		} else {
			return odLEFT;
		}
	} else {
		if(rightFlag) {
			return odRIGHT;
		}
	}

	// check ultrasonic values now
	if((lastFLDistance == -1) || (lastFRDistance ==-1)) {
		// something is wrong, try to measure the distance again
		return odUNKNOWN;
	} else {
		if(lastFLDistance == lastFRDistance) {
			return odBOTH;
		} else {
			if(lastFLDistance < lastFRDistance) {
				return odLEFT;
			} else {
				return odRIGHT;
			}
		}
	}

	return odNONE;
}

bool RobotDistanceSensor::getFrontLeftIRDetected() {
	return !digitalRead(irFrontLeftPin);
}

bool RobotDistanceSensor::getFrontRightIRDetected() {
	return !digitalRead(irFrontRightPin);
}

bool RobotDistanceSensor::getFrontAbyssDetected() {
	return digitalRead(abyssLeftPin) || digitalRead(abyssRightPin);
}

bool RobotDistanceSensor::getFrontLeftAbyssDetected() {
	return digitalRead(abyssLeftPin);
}

bool RobotDistanceSensor::getFrontRightAbyssDetected() {
	return digitalRead(abyssRightPin);
}

void RobotDistanceSensor::sendDistancesSensorsState(bool forceSideDistances) {

	// should we look for the side distances by rotating the UltraSonic sensor?
	if(forceSideDistances) {
		// do a complete refresh of the distances turning the US sensor right and left
		querySideDistances(true);
	} else {
		// return a quick data not waiting for the US sensor head turns
		sendFrontDistance(getFrontDistance(), 'F');
	}

	char messageBuffer[7];

	// send the IR sensors state
	snprintf(messageBuffer, 7, "RO%d%d%d%d",
			getFrontLeftIRDetected(),
			getFrontLeftAbyssDetected(),
			getFrontRightAbyssDetected(),
			getFrontRightIRDetected());
	robotConnector->sendMessage(messageBuffer);

}

void RobotDistanceSensor::sendFrontDistance(int8_t distance, char direction) {

	char messageBuffer[5] = "RF--";

	messageBuffer[1] = direction;

	if(distance >-1) {
		// front distance is available
		// convert it to string with the leading zeroes
		// do not exceed 99 cm
		if(distance > 99) distance = 99;

		snprintf(messageBuffer, 5, "RF%02d", distance);
	}
	robotConnector->sendMessage(messageBuffer);
}
