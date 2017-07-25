#ifndef RobotDistanceSensor_h
#define RobotDistanceSensor_h

#include "Arduino.h"
#include <Servo.h>
#include "TaskInterface.h"
#include "RobotConnector.h"

/*
 * A set of the distance sensor states:
 * dsIdle - doing nothing
 * dsMeasuringFR - turning sensor to the Front-Right position
 * dsMeasuringFL - turning sensor to the Front-Left position
 * dsMeasuringFF - turning sensor to the Front position
 */
enum DSStates {
	dsIdle, dsMeasuringFR, dsMeasuringFL, dsMeasuringFF
};

/*
 * Possible return values of the front obstacle detection
 */
enum FrontObstacleStatus {
	foUNKNOWN,  // impossible to detect as for now
	foOBSTACLE, // obstacle (or abyss) detected
	foFREE      // the road is free
};

/*
 * Possible obstacle positions
 */
enum ObstacleDirections {
	odUNKNOWN, // impossible to detect as for now
	odRIGHT, // obstacle is to the right
	odLEFT,  // obstacle is to the left
	odBOTH,  // obstacle is precisely in front
	odNONE   // no obstacle detected
};

class RobotDistanceSensor: public TaskInterface {
private:
	// angle positions of the sensor servo for different directions
	static const uint8_t F_POS = 85; // adjust this value to make the head look straight
	static const uint8_t FL_POS = F_POS+50;
	static const uint8_t FR_POS = F_POS-50;

	// the time for servo in ms to turn the head by 45 degree
	// minimum is ~100 ms (from the servo specs: Operating Speed (4.8V no load): 0.12sec/60 degrees)
	// choosing here a safe and graceful value
	static const uint16_t SERVO_DELAY = 300;

	// do not bother the sensor more frequently than once per 200 ms
	static const uint16_t FRONT_MEASURE_VALIDITY = 200;

	// the minimum acceptable distance in front (cm)
	static const uint8_t MIN_DISTANCE = 20;

	// the sensors state
	DSStates dsState = dsIdle;

	uint8_t triggerPin;
	uint8_t echoPin;

	uint8_t abyssLeftPin;
	uint8_t abyssRightPin;
	uint8_t irFrontLeftPin;
	uint8_t irFrontRightPin;

	Servo usServo;

	// temporary storage for the last measured distance
	int8_t lastFDistance = -1;
	int8_t lastFLDistance = -1;
	int8_t lastFRDistance = -1;

	// the timestamp of the last front distance measurement
	long lastFDistanceTimeStamp = 0;

	// reference to the robot Wi-Fi connector
	// sometimes we need to send measured distances over WiFi
	RobotConnector* robotConnector;

	// is set to true when after measuring distances they need to be sent to the RC client
	bool sendDistances = false;

	// just reads the distance from the sensor regardless of the servo position
	// returns the distance in cm
	int8_t getDistance();

	// send one of the front distances to the Remote Control client
	// Direction can be 'F', 'L', 'R' - front, left, right
	void sendFrontDistance(int8_t distance, char direction);

public:
	RobotDistanceSensor(uint8_t in_servoPin, uint8_t in_triggerPin, uint8_t in_echoPin,
			uint8_t in_abyssLeftPin, uint8_t in_abyssRightPin, uint8_t in_IRFrontLeftPin, uint8_t in_IRFrontRightPin,
			RobotConnector* in_robotConnector);
	virtual ~RobotDistanceSensor();

	/*
	 * This method will handle asynchronous queued tasks
	 * servo can't move instantly to different positions
	 * that's why we need to introduce states and spend some time
	 * on turning the servo and measuring the distances
	 */
	virtual void processTask();

	/*
	 * Returns:
	 *   foUNKNOWN - impossible to detect as for now
	 *   foOBSTACLE - obstacle (or abyss) detected by at least one sensor
         *   foFREE - the road is free
	 */
	FrontObstacleStatus isObstacleInFront();

	/*
	 * Returns the direction where an obstacle was detected
	 */
	ObstacleDirections getObstacleDirection();

	/*
	 * Return a distance in front of the robot.
	 * Front distance is measured instantly if possible (if the head is turned to the
	 * appropriate direction) but not more frequent then once per 200 ms
	 */
	int8_t getFrontDistance();

	/*
	 * These two methods return the distances if they are available or -1 if not.
	 * Front-Right and Front-Left distances are not measured instantly.
	 * Cast querySideDistances() to refresh them.
	 */
	int8_t getLastFrontLeftDistance();
	int8_t getLastFrontRightDistance();

	/*
	 * Initiate the measurement of the side distances (front-left and front-rihgt)
	 * this method performs a sequence of actions on turning the head and measuring the distances
	 * this work can take up to 500 ms
	 * If sendDistances == true - send measured distances to the Remote Control client
	 */
	void querySideDistances(bool in_sendDistances);

	// Front obstacle detectors getters
	bool getFrontLeftIRDetected();
	bool getFrontRightIRDetected();

	// Abyss detection sensors query block
	// true is returned if abyss is detected
	bool getFrontAbyssDetected();
	bool getFrontLeftAbyssDetected();
	bool getFrontRightAbyssDetected();

	// Send the distance sensors measurements to the Remote Control client
	// If forceSideDistances true - measure also side distances rotating the US sensor
	void sendDistancesSensorsState(bool forceSideDistances);
};

#endif //#ifndef RobotDistanceSensor_h
