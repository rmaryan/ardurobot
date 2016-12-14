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
		if (Serial.available() > 0) {
			remoteCommand = Serial.read();

			// "R" means "Toggle Remote Control Mode"
			if (remoteCommand == 'R') {
				robotMotors->fullStop();
				robotVoice->queueSound(sndOK);
				if (currentAIMode == modRemoteControl) {
					// switch to the idle mode
					currentAIMode = modIdle;

					// notify the RC application about the acceptance of the command
					// LED will be turned OFF on the RC dashboard
					Serial.write("RCMODE off\n");
				} else {
					// engage the RC mode
					currentAIMode = modRemoteControl;

					// notify the RC application about the acceptance of the command
					// LED will be turned ON on the RC dashboard
					Serial.write("RCMODE on\n");
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
				Serial.write("dbg Moving forward\n");
				robotMotors->driveForward(255, 600);
				break;
			case 'A':
				Serial.write("dbg Turning Left\n");
				robotMotors->turnLeft(255, 600);
				break;
			case 'D':
				Serial.write("dbg Turning Right\n");
				robotMotors->turnRight(255, 600);
				break;
			case 'S':
				Serial.write("dbg STOP\n");
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

			// obstacle detection distance (cm)
			const uint8_t MIN_DISTANCE = 20;
			uint8_t distance = robotDistanceSensor->getFrontDistance();

			Serial.println(distance);

			// is there an obstacle in front of the robot?
			if(distance < MIN_DISTANCE) {
				robotMotors->fullStop();
				robotVoice->queueSound(sndQuestion);

				//TODO complete the collision avoidance code

//				float frDistance = robotDistanceSensor->getFrontRightDistance();
//				float flDistance = robotDistanceSensor->getFrontLeftDistance();
//
//				if(frDistance > flDistance) {
//					// turn right
//					robotMotors->turnRight(255, 1000);
//				} else {
//					// turn left
//					robotMotors->turnLeft(255, 1000);
//				}
//				scheduleTimedTask(1200);
			} else {
				robotMotors->driveForward(255, 200);
				scheduleTimedTask(100);
			}
		}
	}
}

void RobotAI::abyssDetected() {
	// triggered when table edge or other abyss is detected
	abyssDetectedFlag = true;
}
