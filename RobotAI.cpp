/*
 * RobotAI.cpp
 *
 */

#include "RobotAI.h"

RobotAI::RobotAI(RobotMotors* in_robotMotors, RobotDistanceSensor* in_robotDistanceSensor, RobotLights* in_robotLights,
		RobotVoice* in_robotVoice, RobotConnector* in_robotConnector) {
	robotMotors = in_robotMotors;
	robotDistanceSensor = in_robotDistanceSensor;
	robotLights = in_robotLights;
	robotVoice = in_robotVoice;
	robotConnector = in_robotConnector;

	// Say hello
	robotVoice->queueSound(sndHello);

	// kick off the heartbeat
	scheduleTimedTask(1000);
}

RobotAI::~RobotAI() {
	;
}

void RobotAI::processTask() {
	// this is an emergency - surface edge or other abyss detected
	if (abyssDetectedFlag) {
		// remove out extra abyss detection
		if(!abyssDetectedProcessing) {
			// check if the sensors really can't see the floor
			if(robotDistanceSensor->getFrontAbyssDetected()) {
				robotMotors->fullStop();
				robotVoice->queueSound(sndScared);
				abyssDetectedProcessing = true;
			}
		}
		abyssDetectedFlag = false;
	}

	// Check if any remote commands are available - there can be requests to change the mode
	// here and further we are a bit ignorant about the real command length
	// we assume that in any case - this is a proper null-terminated string
	char* remoteCommand = NULL;
	if (robotConnector->commandAvailable()) {
		// get the reference to the remote command
		remoteCommand = robotConnector->getCommand();

		// Check if this is a command to change the mode
		if (remoteCommand[0] == 'M') {
			// stop any current activities
			robotMotors->fullStop();
			robotVoice->queueSound(sndOK);

			switch(remoteCommand[1]) {
			case 'I':
				// switch to the idle mode
				switchToModeIdle();
				robotConnector->sendMessage("MI");
				break;
			case 'A':
				switchToModeAI();
				robotConnector->sendMessage("MA");
				break;
			case 'S':
				switchToModeScenario();
				robotConnector->sendMessage("MS");
				break;
			case 'R':
				switchToModeRC();
				robotConnector->sendMessage("MR");
				break;
			}
		}
	}

	switch (currentAIMode) {
	case modIdle:
		abyssDetectedProcessing = false;
		break; //do nothing
	case modRemoteControl: {
		if(abyssDetectedProcessing) {
			// TODO this is to be rewritten once more advanced remote control will be ready
			// drive backward and wait a bit, then switch to idle mode
			robotMotors->driveBackward(160, 600);
			abyssDetectedProcessing = false;
			currentAIMode = modIdle;
		} else {
			switch (remoteCommand[0]) {
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
			case 'L':
				// Lights control commands
				// TODO to be implemented
				break;
			case 'R':
				// Distance querying commands
				if(remoteCommand[1]=='0') {
					// do a complete refresh of the distances turning the US sensor right and left
					// TODO complete this
					// Send:
					// RLxxx - left distance, where xxx is a distance in millimeters with the leading zeros
					// RFxxx - front distance
					// ROABCD - obstacle detectors state ("1"s or "0"s), <BR> A - left-ahead, B - left-edge, C - right-edge, D - right-ahead
				} else
					if (remoteCommand[1]=='1') {
						// return a quick data not waiting for the US sensor head turns

						char messageBuffer[7] = "RF--";

						int8_t frontDistance = robotDistanceSensor->getFrontDistance();
						if(frontDistance >-1) {
							// front distance is available
							// convert it to string with the leading zeroes
							// do not exceed 99 cm
							if(frontDistance > 99) frontDistance = 99;
							snprintf(messageBuffer, 5, "RF%02d", frontDistance);
						}
						robotConnector->sendMessage(messageBuffer);

						// send the IR sensors state
						snprintf(messageBuffer, 7, "RO%d%d%d%d",
								robotDistanceSensor->getFrontLeftIRDetected(),
								robotDistanceSensor->getFrontLeftAbyssDetected(),
								robotDistanceSensor->getFrontRightAbyssDetected(),
								robotDistanceSensor->getFrontRightIRDetected());
						robotConnector->sendMessage(messageBuffer);
					}
				break;
			case 'X':
				// Motors speed direct control commands
				// TODO to be implemented
				// XAAABBB
				// Set the drives speed to AAA (left) and BBB (right). The speed is in range 000 - 511. 000 is full reverse, 511 means full ahead.
				break;
			}
		}
		break;
	}
	case modScript:
		//scripted moving mode

		// if abyss detected - just stop
		if(abyssDetectedProcessing) {
			currentAIMode = modIdle;
			abyssDetectedProcessing = false;
		} else {
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
		}
		break;
	case modAI:
		// a simple wandering around mode, with the use of the distance sensor
		if (reachedDeadline()) {

			switch (currentAIState) {
			case stateAI_GO: {
				// check for abyss
				if(abyssDetectedProcessing) {
					// if abyss detected - stop, go backward, then turn according to the distances
					robotMotors->driveBackward(MOTOR_DRIVE_SPEED, 900);
					scheduleTimedTask(1000);
					currentAIState = stateAI_QueryDistances;
				} else {
					FrontObstacleStatus frontObstacleStatus = robotDistanceSensor->isObstacleInFront();
					if(frontObstacleStatus == foUNKNOWN) {
						// can't measure the distance as for now
						// wait a little
						scheduleTimedTask(300);
					} else
						if(frontObstacleStatus == foOBSTACLE) {
							robotMotors->fullStop();
							robotVoice->queueSound(sndQuestion);
							robotDistanceSensor->querySideDistances();

							currentAIState = stateAI_QueryDistances;
							scheduleTimedTask(3000);
						} else {
							// the road is clear - go ahead!
							robotMotors->driveForward(MOTOR_DRIVE_SPEED, 350);
							scheduleTimedTask(300);
						}
				}
				break;
			}
			case stateAI_QueryDistances: {

				ObstacleDirections obstacleDirection = robotDistanceSensor->getObstacleDirection();


				if(obstacleDirection == odUNKNOWN) {
					// something is wrong, try to measure the distance again
					robotDistanceSensor->querySideDistances();
					scheduleTimedTask(3000);
				} else {
					// we assume we already escaped the abyss
					// if not - it may be dangerous to move backward again
					// START PANICKING!!!
					if(robotDistanceSensor->getFrontAbyssDetected()) {
						robotVoice->queueSound(sndScared);
						scheduleTimedTask(2000);
					} else {
						abyssDetectedProcessing = false;

						// need to turn right or left
						if(obstacleDirection == odBOTH) {
							// choose the direction randomly
							if(millis() % 2 == 0) {
								robotMotors->turnRight(MOTOR_TURN_SPEED, MOTOR_TURN_DURATION);
							} else {
								robotMotors->turnLeft(MOTOR_TURN_SPEED, MOTOR_TURN_DURATION);
							}
						} else
							if(obstacleDirection == odLEFT) {
								robotMotors->turnRight(MOTOR_TURN_SPEED, MOTOR_TURN_DURATION);
							} else {
								robotMotors->turnLeft(MOTOR_TURN_SPEED, MOTOR_TURN_DURATION);
							}
						currentAIState = stateAI_Turning;

						scheduleTimedTask(MOTOR_TURN_DURATION);
					}
				}
				break;
			}
			case stateAI_Turning:
				// turning has been finished time to GO
				currentAIState = stateAI_GO;
				break;
			}
		}
	}
}

void RobotAI::switchToModeIdle() {
	currentAIMode = modIdle;
}
void RobotAI::switchToModeAI() {
	currentAIMode = modAI;
}
void RobotAI::switchToModeScenario() {
	currentAIMode = modScript;
	currentScriptLine = 0;
}
void RobotAI::switchToModeRC() {
	currentAIMode = modRemoteControl;
}
