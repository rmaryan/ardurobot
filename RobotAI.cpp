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

	// Check if to toggle the remote control mode
	int remoteCommand = 0;
	if (Serial.available() > 0) {
		remoteCommand = Serial.read();
		
		// "R" means "Toggle Remote Control Mode"
		if(remoteCommand == 'R') {
			robotMotors->fullStop();
			robotVoice->queueSound(sndOK);
			if(currentAIMode==modRemoteControl) {
				// switch to the idle mode
				currentAIMode = modIdle;
				
				// notify the RC application about the acceptance of the command
				// LED will be turned OFF on the RC dashboard
				Serial.write("RCMODE off\n");
			} else {
				// engage the RC mode
				currentAIMode=modRemoteControl;
				
				// notify the RC application about the acceptance of the command
				// LED will be turned ON on the RC dashboard
				Serial.write("RCMODE on\n");
			}
		}
	}


	switch(currentAIMode) {

	case modIdle: break; //do nothing
	
	case modRemoteControl:
		switch(remoteCommand) {
		case 0: break;
		case 'W':
			// Moving forward
			robotMotors->driveForward(255, 600);
			break;
		case 'A':
			// Turning Left
			robotMotors->turnLeft(255, 600);
			break;
		case 'D':
			// Turning Right
			robotMotors->turnRight(255, 600);
			break;
		case 'S':
			// STOP
			robotMotors->fullStop();
			break;
		}
		break;

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
