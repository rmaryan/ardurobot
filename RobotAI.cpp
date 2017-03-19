/*
 * RobotAI.cpp
 *
 */

#include "RobotAI.h"

RobotAI::RobotAI(RobotMotors* in_robotMotors, RobotDistanceSensor* in_robotDistanceSensor, RobotLights* in_robotLights,
		RobotVoice* in_robotVoice) {
	robotMotors = in_robotMotors;
	robotDistanceSensor = in_robotDistanceSensor;
	robotLights = in_robotLights;
	robotVoice = in_robotVoice;

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
