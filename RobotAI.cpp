/*
 * RobotAI.cpp
 *
 */

#include "RobotAI.h"

RobotAI::RobotAI(RobotMotors* in_robotMotors, RobotVoice* in_robotVoice) {
	robotMotors = in_robotMotors;
	robotVoice = in_robotVoice;

	// Say hello
	robotVoice->queueSound(sndHello);

	// kick off the heartbeat
	scheduleTimedTask(1000);
}

RobotAI::~RobotAI() {

}

void RobotAI::processTask() {
	switch(currentAIMode) {
	case modIdle: break; //do nothing
	case modAI:
		//autonomous mode
		if(reachedDeadline()) {
			//get the next command from the script
			if(currentScriptLine<SCRIPT_LENGTH) {
				uint16_t actionLength = aiScript[currentScriptLine].duration + 200;
				switch(aiScript[currentScriptLine].action) {
				case W:
					//just do nothing
					break;
				case MF:
					// Motors Forward
					robotMotors->driveForward(255, actionLength);
					break;
				case ML:
					// Motors Left
					robotMotors->turnLeft(255, actionLength);
					break;
				case MR:
					// Motors Right
					robotMotors->turnRight(255, actionLength);
					break;
				case VH:
					// Voice "Hello"
					robotVoice->queueSound(sndHello);
					break;
				case VQ:
					// Voice "Question"
					robotVoice->queueSound(sndQuestion);
					break;
				case VO:
					// Voice "OK"
					robotVoice->queueSound(sndOK);
					break;
				}
				scheduleTimedTask(aiScript[currentScriptLine].duration);
				currentScriptLine++;
			} else {
				// end of script, start idling
				robotMotors->fullStop();
				robotVoice->queueSound(sndQuestion);
				currentAIMode = modIdle;
			}

		}
	}
}
