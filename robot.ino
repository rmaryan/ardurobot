/*
 * robot.ino
 *
 */

#include "Arduino.h"
#include "TaskInterface.h"
#include "RobotMotors.h"
#include "RobotDistanceSensor.h"
#include "RobotLights.h"
#include "RobotVoice.h"
#include "RobotAI.h"

/****************************************************************************
 * This is a block which stores allocations of all Arduino pins used by different functions.
 * It is better to have this stored in a single place, rather than spreading it all over the code.
 */
// PIN 0 used by ESP13
// PIN 1 used by ESP13
// PIN 2 FREE
// PIN 3 used by the motor shield (drive 2)
// PIN 4 used by the motor shield
// PIN 5 FREE
// PIN 6 FREE
// PIN 7 used by the motor shield
// PIN 8 used by the motor shield
const uint8_t US_SERVO_PIN = 9;
const uint8_t VOICE_PIN = 10;
// PIN 11 is used by the motor shield (drive 1)
// PIN 12 is used by the motor shield
const uint8_t US_ECHO_PIN = 22;
const uint8_t US_TRG_PIN = 23;

const uint8_t ABYSS_PIN = 100; //TODO assign a real pin with interrupts support
const uint8_t LED_DATA_PIN = 100; //TODO assign a real pin
const uint8_t LED_SYNC_PIN = 100; //TODO assign a real pin
const uint8_t LED_LATCH_PIN = 100;  //TODO assign a real pin
/*****************************************************************************/

const uint8_t TASKS_COUNT = 5;

// the list of the robots tasks to be executed in the main loop
static TaskInterface* (robotTasks[TASKS_COUNT]);

void setup() {
	// open a serial connection for the remote control
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// Define the entities which have own time slice in the main loop (tasks)
	RobotMotors* motors = new RobotMotors();
	RobotDistanceSensor* robotDistanceSensor = new RobotDistanceSensor(US_SERVO_PIN, US_TRG_PIN, US_ECHO_PIN);
	RobotLights* robotLights = new RobotLights(LED_DATA_PIN, LED_SYNC_PIN, LED_LATCH_PIN);
	RobotVoice*  robotVoice = new RobotVoice(VOICE_PIN);
	RobotAI*     robotAI = new RobotAI(motors, robotDistanceSensor, robotLights, robotVoice, ABYSS_PIN);

	// create the tasks list
	robotTasks[0] = motors;
	robotTasks[1] = robotDistanceSensor;
	robotTasks[2] = robotLights;
	robotTasks[3] = robotVoice;
	robotTasks[4] = robotAI;
}

void loop() {
	// in the main loop just kick the processing method for each robot's task
	for(uint8_t i=0; i<TASKS_COUNT; i++) {
		robotTasks[i]->processTask();
	}
}
