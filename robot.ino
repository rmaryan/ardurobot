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
#include "RobotConnector.h"

/****************************************************************************
 * This is a block which stores allocations of all Arduino pins used by different functions.
 * It is better to have this stored in a single place, rather than spreading it all over the code.
 */
// PIN 0 used by Arduino USB port
// PIN 1 used by Arduino USB port
// PIN 2 FREE
// PIN 3 used by the motor shield (drive 2)
// PIN 4 used by the motor shield
// PIN 5 FREE
// PIN 6 FREE
// PIN 7 used by the motor shield
// PIN 8 used by the motor shield
const uint8_t US_SERVO_PIN = 9; // ultrasonic sensor servo control pin
const uint8_t VOICE_PIN = 10; // voice beeper PWM pin
// PIN 11 is used by the motor shield (drive 1)
// PIN 12 is used by the motor shield
// PIN 14 used by ESP13 (Serial3 - remote control)
// PIN 15 used by ESP13 (Serial3 - remote control)
const uint8_t ABYSS_RIGHT_PIN = 20; // IR sensor abyss detection right side
const uint8_t ABYSS_LEFT_PIN = 21;  // IR sensor abyss detection left side
const uint8_t US_ECHO_PIN = 22; // ultrasonic sensor echo pin
const uint8_t US_TRG_PIN = 23;  // ultrasonic sensor trigger pin
const uint8_t IR_FRONT_RIGHT_PIN = 51;  // infrared obstacle detector
const uint8_t IR_FRONT_LEFT_PIN = 53;  // infrared obstacle detector

const uint8_t LED_DATA_PIN = 100; //TODO assign a real pin
const uint8_t LED_SYNC_PIN = 100; //TODO assign a real pin
const uint8_t LED_LATCH_PIN = 100;  //TODO assign a real pin
/*****************************************************************************/

const uint8_t TASKS_COUNT = 6;

// the list of the robots tasks to be executed in the main loop
static TaskInterface* (robotTasks[TASKS_COUNT]);

// Define the entities which have own time slice in the main loop (tasks)
RobotMotors* motors;
RobotDistanceSensor* robotDistanceSensor;
RobotLights* robotLights;
RobotVoice*  robotVoice;
RobotConnector* robotConnector;
RobotAI*     robotAI;


void setup() {
	// open a serial port for the debug messages
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// open a serial connection for the remote control
	Serial3.begin(9600);

	// Define the entities which have own time slice in the main loop (tasks)
	motors = new RobotMotors();
	robotConnector = new RobotConnector();
	robotDistanceSensor = new RobotDistanceSensor(US_SERVO_PIN, US_TRG_PIN, US_ECHO_PIN,
			ABYSS_LEFT_PIN, ABYSS_RIGHT_PIN, IR_FRONT_LEFT_PIN, IR_FRONT_RIGHT_PIN, robotConnector);
	robotLights = new RobotLights(LED_DATA_PIN, LED_SYNC_PIN, LED_LATCH_PIN);
	robotVoice = new RobotVoice(VOICE_PIN);
	robotAI = new RobotAI(motors, robotDistanceSensor, robotLights, robotVoice, robotConnector);

	// create the tasks list
	robotTasks[0] = motors;
	robotTasks[1] = robotDistanceSensor;
	robotTasks[2] = robotLights;
	robotTasks[3] = robotVoice;
	robotTasks[4] = robotConnector;
	robotTasks[5] = robotAI;

	// register the abyss detection interrupts handler
	// this handler is called each time sensors detect an abyss in front of the robot
	attachInterrupt(digitalPinToInterrupt(ABYSS_LEFT_PIN), abyssDetected, FALLING);
	attachInterrupt(digitalPinToInterrupt(ABYSS_RIGHT_PIN), abyssDetected, FALLING);
}

void loop() {
	// in the main loop just kick the processing method for each robot's task
	for(uint8_t i=0; i<TASKS_COUNT; i++) {
		robotTasks[i]->processTask();
	}
}

// the abyss detection interrupt handler
// it is not easy to assign a class member as a handler for the interrupt
// so we just get this out from the AI class for the  sake of the simplicity
// See here for the details:http://forum.arduino.cc/index.php?topic=41713.0
void abyssDetected() {
	// triggered when table edge or other abyss is detected
	robotAI->abyssDetectedFlag = true;
}
