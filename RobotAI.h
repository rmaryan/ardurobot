/*
 * RobotAI.h
 *
 */

#ifndef ROBOTAI_H_
#define ROBOTAI_H_

#include "TaskInterface.h"
#include "RobotMotors.h"
#include "RobotLights.h"
#include "RobotVoice.h"
#include "RobotDistanceSensor.h"
#include "RobotConnector.h"

/*
 * The list of the possible AI modes
 * modIdle - doing nothing
 * modRemoteControl - executing remote commands
 * modAI - autonomous mode
 */
enum AIModes {
	modIdle, modRemoteControl, modScript, modAI
};

/*
 * The list of the AI states - they are used in "modAI" mode only.
 * modAI_GO - go forward until an obstacle detected
 * modAI_QueryDistances - waiting until the side distances are measured
 * modAI_Turning - turning to the new course
 */
enum AIStates {
	stateAI_GO, stateAI_QueryDistances, stateAI_Turning
};


/*
 * Possible actions which can be taken by the AI
 * W - wait
 * MF - motors forward
 * ML - motors turn left
 * MR - motors turn right
 * VH - voice say hello
 * VQ - voice say question
 * VO - voice say OK
 */
enum AIAction {
	W, MF, ML, MR, VH, VQ, VO
};

/*
 * Container for a single script line. Something to be executed during some period of time
 */
struct AIScriptLine {
	AIAction action;
	uint16_t duration;
};

class RobotAI: public TaskInterface {
private:
	// references to the other robot modules
	RobotMotors* robotMotors;
	RobotDistanceSensor* robotDistanceSensor;
	RobotLights* robotLights;
	RobotVoice* robotVoice;
	RobotConnector* robotConnector;

	// when this flag is true - abyss was detected but the case was not handled yet
	bool abyssDetectedProcessing = false;

	// AI modes states and parameters
	AIModes currentAIMode = modAI;
	AIStates currentAIState = stateAI_GO;
	static const uint8_t MOTOR_DRIVE_SPEED = 180;
	static const uint8_t MOTOR_TURN_SPEED = 180;
	static const uint16_t MOTOR_TURN_DURATION = 1200;

	uint8_t currentScriptLine = 0;
	/*
	 * Definition of some simple scenario to be followed by the robot in autonomous mode.
	 * Robot will perform this script after being switched on, then will stop and idle forever
	 */
	static const uint8_t SCRIPT_LENGTH = 8;
	AIScriptLine aiScript[SCRIPT_LENGTH] = { { VQ, 1000 }, // beep the question sound
			{ W, 1000 },  // wait one more second
			{ VO, 1000 }, // beep the "OK" sound
			{ MF, 6000 }, // move forward for 6 seconds
			{ MR, 6000 }, // turn right for 6 seconds
			{ MF, 6000 }, // move forward for 6 seconds
			{ ML, 12000 }, // turn left for 12 seconds
			{ MF, 3000 }   // move forward for 3 seconds

	};

public:
	// the value of true is pushed to this variable by the external interrupt handler if the sensors
	// detect abyss in front of the robot
	volatile bool abyssDetectedFlag = false;

	/*
	 * Robot AI constructor initializes the AI and stores the references to other robot modules.
	 */
	RobotAI(RobotMotors* in_robotMotors, RobotDistanceSensor* in_robotDistanceSensor, RobotLights* in_robotLights,
			RobotVoice* in_robotVoice, RobotConnector* in_robotConnector);

	/*
	 * Cease all AI activities gracefully
	 */
	virtual ~RobotAI();

	/*
	 * This function is called by a task scheduler each time the world ticks.
	 * Here the AI controls the time and choose the next actions to do.
	 */
	virtual void processTask();

	/*
	 * These functions switch the AI module between the working modes.
	 */
	virtual void switchToModeIdle();
	virtual void switchToModeAI();
	virtual void switchToModeScenario();
	virtual void switchToModeRC();
};

#endif /* ROBOTAI_H_ */
