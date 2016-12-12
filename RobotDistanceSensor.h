#ifndef RobotDistanceSensor_h
#define RobotDistanceSensor_h

#include "Arduino.h"
#include <Servo.h>
#include "TaskInterface.h"

class RobotDistanceSensor: public TaskInterface {
private:
	uint8_t triggerPin;
	uint8_t echoPin;

	Servo usServo;

	// temporary storage for the last measured distance
	// these value expire in 1 second after the measurement
	int8_t lastFDistance = -1;
	int8_t lastFLDistance = -1;
	int8_t lastFRDistance = -1;

	// just reads the distance from the sensor
	int8_t getDistance();

public:
	RobotDistanceSensor(uint8_t in_servoPin, uint8_t in_triggerPin, uint8_t in_echoPin);
	virtual ~RobotDistanceSensor();

	// this method will handle asynchronous queued tasks
	virtual void processTask();

	// these three methods returns the distances if they are available or -1 if not
	int8_t getFrontDistance();
	int8_t getFrontLeftDistance();
	int8_t getFrontRightDistance();

	// initialize the measurement of the side distances (front-left, front, front-rihgt)
	// this method performs a sequence of actions on turining the head and measuring the distances
	// this work can take up to 500 ms
	void querySideDistances();
};

#endif //#ifndef RobotDistanceSensor_h
