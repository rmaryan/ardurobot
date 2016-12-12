/*
 * robot.ino
 *
 */

#include "Arduino.h"
#include "TaskInterface.h"
#include "RobotMotors.h"
#include "RobotVoice.h"
#include "RobotAI.h"

/*
 * This is a block which stores allocations of all Arduino pins used by different functions.
 * It is better to have this stored in a single place, rather than spreading it all over the code.
 */
const uint8_t VOICE_PIN = 9;
const uint8_t LEFT_DRIVE_PIN = 10;
const uint8_t RIGHT_DRIVE_PIN = 11;

const uint8_t TASKS_COUNT = 3;

// the list of the robots tasks to be executed in the main loop
static TaskInterface* (robotTasks[TASKS_COUNT]);

// Define the entities which have own time slice in the main loop (tasks)
static RobotMotors motors(LEFT_DRIVE_PIN, RIGHT_DRIVE_PIN);
static RobotVoice robotVoice(VOICE_PIN);
static RobotAI robotAI(&motors, &robotVoice);

void setup() {
	// create the tasks list
	robotTasks[0] = &motors;
	robotTasks[1] = &robotVoice;
	robotTasks[2] = &robotAI;
}

void loop() {
	// in the main loop just kick the processing method for each robot's task
	for (uint8_t i = 0; i < TASKS_COUNT; i++) {
		robotTasks[i]->processTask();
	}
}
