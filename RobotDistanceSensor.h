#ifndef RobotDistanceSensor_h
#define RobotDistanceSensor_h

#include "Arduino.h"
#include <Servo.h>
#include "TaskInterface.h"

/*
 * A set of the distance sensor states:
 * dsIdle - doing nothing
 * dsMeasuringFR - turning sensor to the Front-Right position
 * dsMeasuringFL - turning sensor to the Front-Left position
 * dsMeasuringFF - turning sensor to the Front position
 * dsResultsReady - results are ready, waiting for the expiration
 */
enum DSStates {
	dsIdle, dsMeasuringFR, dsMeasuringFL, dsMeasuringFF, dsResultsReady
};

class RobotDistanceSensor: public TaskInterface {
private:
	// angle positions of the sensor servo for different directions
	static const uint8_t F_POS = 90;
	static const uint8_t FL_POS = 140;
	static const uint8_t FR_POS = 40;

	// the duration in ms of the validity of the measured distances
	static const uint16_t VALIDITY_PERIOD = 1000;

	// the time for servo in ms to turn the head by 45 degree
	// taken from the servo specs: Operating Speed (4.8V no load): 0.12sec/60 degrees
	static const uint16_t SERVO_DELAY = 100;

	// the sensors state
	DSStates dsState = dsIdle;

	uint8_t triggerPin;
	uint8_t echoPin;

	Servo usServo;

	// temporary storage for the last measured distance
	// these value expire in 1 second after the measurement
	int8_t lastFDistance = -1;
	int8_t lastFLDistance = -1;
	int8_t lastFRDistance = -1;

	// just reads the distance from the sensor regardless of the servo position
	// returns the distance in cm
	int8_t getDistance();

public:
	RobotDistanceSensor(uint8_t in_servoPin, uint8_t in_triggerPin, uint8_t in_echoPin);
	virtual ~RobotDistanceSensor();

	/*
	 * This method will handle asynchronous queued tasks
	 * servo can't move instantly to different positions
	 * that's why we need to introduce states and spend some time
	 * on turning the servo and measuring the distances
	 */
	virtual void processTask();

	/*
	 * These three methods returns the distances if they are available or -1 if not
	 * distance is being measured instantly if possible (if the head is turned to the
	 * appropriate direction
	*/
	int8_t getFrontDistance();
	int8_t getFrontLeftDistance();
	int8_t getFrontRightDistance();

	 /*
	  * Initiate the measurement of the side distances (front-left, front, front-rihgt)
	  * this method performs a sequence of actions on turining the head and measuring the distances
	  * this work can take up to 500 ms
	  */
	void querySideDistances();
};

#endif //#ifndef RobotDistanceSensor_h
