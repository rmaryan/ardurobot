#include "RobotDistanceSensor.h"

RobotDistanceSensor::RobotDistanceSensor(uint8_t in_servoPin, uint8_t in_triggerPin, uint8_t in_echoPin,
		uint8_t in_abyssLeftPin, uint8_t in_abyssRightPin, uint8_t in_IRFrontLeftPin, uint8_t in_IRFrontRightPin)
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

void RobotDistanceSensor::querySideDistances() {
	// start measuring from the front-right
	dsState = dsMeasuringFR;

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
