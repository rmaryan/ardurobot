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
			// drive backward and wait a bit
			robotMotors->driveBackward(160, 600);
			abyssDetectedProcessing = false;
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
				robotMotors->driveBackward(255, 600);
				break;
			case 'L':
				// Lights control commands
				if(strlen(remoteCommand)==3) {
					bool turnOn = (remoteCommand[2] == '1');

					switch (remoteCommand[1]) {
					case 'F':
						robotLights->turnFrontLED(turnOn);
						robotConnector->sendMessage(remoteCommand);
						break;
					case 'R':
						robotLights->turnRearLED(turnOn);
						robotConnector->sendMessage(remoteCommand);
						break;
					case 'S':
						robotLights->turnSideLED(turnOn);
						robotConnector->sendMessage(remoteCommand);
						break;
					}
				}
				break;
			case 'R':
				// Distance querying command
				robotDistanceSensor->sendDistancesSensorsState(remoteCommand[1]=='0');
				break;
			case 'X':
				// Motors speed direct control commands XAAABBB
				// Set the drives speed to AAA (left) and BBB (right).
				// The speed is in range 000 - 511. 000 is full reverse, 511 means full ahead.

				if(strlen(remoteCommand)==7) {
					char speedBuffer[4] = "000";
					uint16_t rightSpeed = 255;
					uint16_t leftSpeed = 255;

					strncpy(speedBuffer, &(remoteCommand[1]),3);
					leftSpeed = atoi(speedBuffer);

					strncpy(speedBuffer, &(remoteCommand[4]),3);
					rightSpeed = atoi(speedBuffer);

					robotMotors->setMotorSpeed(leftSpeed, rightSpeed, 600);
				}
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
							robotDistanceSensor->querySideDistances(false);

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
					robotDistanceSensor->querySideDistances(false);
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
	robotLights->turnSideLED(false);
}
void RobotAI::switchToModeAI() {
	currentAIMode = modAI;
	robotLights->turnSideLED(true);
}
void RobotAI::switchToModeScenario() {
	currentAIMode = modScript;
	currentScriptLine = 0;
	robotLights->turnSideLED(false);
}
void RobotAI::switchToModeRC() {
	currentAIMode = modRemoteControl;
	robotLights->turnSideLED(false);
}
