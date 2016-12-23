/*
 * RobotAI.cpp
 *
 */

#include "RobotAI.h"

RobotAI::RobotAI(RobotMotors* in_robotMotors, RobotDistanceSensor* in_robotDistanceSensor, RobotLights* in_robotLights,
		RobotVoice* in_robotVoice, uint8_t in_abyssPin) {
	robotMotors = in_robotMotors;
	robotDistanceSensor = in_robotDistanceSensor;
	robotLights = in_robotLights;
	robotVoice = in_robotVoice;
	abyssPin = in_abyssPin;

	//attachInterrupt(abyssPin, abyssDetected, FALLING);

	// Say hello
	robotVoice->queueSound(sndHello);

	// kick off the heartbeat
	scheduleTimedTask(1000);
}

RobotAI::~RobotAI() {
	;
}

void RobotAI::processTask() {
	// this is an emergency
	if (abyssDetectedFlag) {
		robotMotors->fullStop();
		robotVoice->queueSound(sndScared);
		abyssDetectedFlag = false;
	} else {

		// Check if to toggle the remote control mode
		int remoteCommand = 0;
		if (Serial3.available() > 0) {
			remoteCommand = Serial3.read();

			// "R" means "Toggle Remote Control Mode"
			if (remoteCommand == 'R') {
				robotMotors->fullStop();
				robotVoice->queueSound(sndOK);
				if (currentAIMode == modRemoteControl) {
					// switch to the idle mode
					currentAIMode = modIdle;

					// notify the RC application about the acceptance of the command
					// LED will be turned OFF on the RC dashboard
					Serial3.write("RCMODE off\n");
				} else {
					// engage the RC mode
					currentAIMode = modRemoteControl;

					// notify the RC application about the acceptance of the command
					// LED will be turned ON on the RC dashboard
					Serial3.write("RCMODE on\n");
				}
			}
		}

		switch (currentAIMode) {
		case modIdle:
			break; //do nothing
		case modRemoteControl: {
			switch (remoteCommand) {
			case 0:
				break;
			case 'W':
				robotMotors->driveForward(255, 600);
				break;
			case 'A':
				robotMotors->turnLeft(255, 600);
				break;
			case 'D':
				robotMotors->turnRight(255, 600);
				break;
			case 'S':
				robotMotors->fullStop();
				break;
			}
			break;
		}
		case modScript:
			//scripted moving mode
			if (reachedDeadline()) {
				//get the next command from the script
				if (currentScriptLine < SCRIPT_LENGTH) {
					uint16_t actionLength = aiScript[currentScriptLine].duration
							+ 200;
					switch (aiScript[currentScriptLine].action) {
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
			break;
		case modAI:
			// a simple wandering around mode, with the use of the distance sensor

			if (reachedDeadline()) {
				// obstacle detection distance (cm)
				const uint8_t MIN_DISTANCE = 20;
				uint8_t distance = robotDistanceSensor->getFrontDistance();

				if(distance == -1) {
					scheduleTimedTask(300);
				} else {

					// is there an obstacle in front of the robot?
					if(distance < MIN_DISTANCE) {
						robotMotors->fullStop();
						robotVoice->queueSound(sndQuestion);

						robotDistanceSensor->querySideDistances();

						scheduleTimedTask(1500);

						//TODO complete the collision avoidance code

					} else {
						robotMotors->driveForward(160, 200);
						scheduleTimedTask(200);
					}
				}
			}
		}
	}
}

void RobotAI::abyssDetected() {
	// triggered when table edge or other abyss is detected
	abyssDetectedFlag = true;
}
