/*
 * RobotAI.h
 *
 */

#ifndef ROBOTAI_H_
#define ROBOTAI_H_

#include "TaskInterface.h"
#include "RobotMotors.h"
#include "RobotVoice.h"

/*
 * The list of the possible AI modes
 * modIdle - doing nothing
 * modAI - autonomous mode
 */
enum AIModes {modIdle, modAI};

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
enum AIAction {W, MF, ML, MR, VH, VQ, VO};

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
	RobotVoice* robotVoice;

	AIModes currentAIMode = modAI;
	uint8_t currentScriptLine = 0;

	/*
	 * Definition of some simple scenario to be followed by the robot in autonomous mode.
	 * Robot will perform this script after being switched on, then will stop and idle forever
	 */
	static const uint8_t SCRIPT_LENGTH = 8;
	AIScriptLine aiScript[SCRIPT_LENGTH] = {
			{VQ, 1000}, // beep the question sound
			{W, 1000},  // wait one more second
			{VO, 1000}, // beep the "OK" sound
			{MF, 6000}, // move forward for 6 seconds
			{MR, 6000}, // turn right for 6 seconds
			{MF, 6000}, // move forward for 6 seconds
			{ML, 12000}, // turn left for 12 seconds
			{MF, 3000}   // move forward for 3 seconds

	};

public:
	/*
	 * Robot AI constructor initializes the AI and stores the references to other robot modules.
	 */
	RobotAI(RobotMotors* in_robotMotors, RobotVoice* in_robotVoice);

	/*
	 * Cease all AI activities gracefully
	 */
	virtual ~RobotAI();

	/*
	 * This function is called by a task scheduler each time the world ticks.
	 * Here the AI controls the time and choose the next actions to do.
	 */
	virtual void processTask();
};

#endif /* ROBOTAI_H_ */
