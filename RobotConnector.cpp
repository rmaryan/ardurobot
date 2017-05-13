/*
 * RobotConnector.cpp
 *
 */

#include "RobotConnector.h"

RobotConnector::RobotConnector() {

}

void RobotConnector::processTask() {
	// do nothing if the buffer contains a complete message
	// which was not processed yet
	if (!messageReady) {
		while (Serial3.available() > 0) {

			int incomingByte = Serial3.read();

			switch (incomingByte) {
			case -1:
				// byte not available - nothing to do
				break;
			case COMMAND_TERM:   // end of message
				messageBuffer[messageSize++] = 0;  // terminating null byte
				messageReady = true;
				break;
			default:
				// check for buffer overflow
				if (messageSize >= (MAX_MSG_SIZE - 1)) {
					//this should never happen, but if it is - just reset everything
					messageSize = 0;
				}
				messageBuffer[messageSize++] = incomingByte;
				break;
			}
		}
	}
}

bool RobotConnector::commandAvailable() {
	return messageReady;
}

char* RobotConnector::getCommand() {
	if (messageReady) {
		messageReady=false;
		messageSize = 0;
		return messageBuffer;
	} else {
		return NULL;
	}
}

void RobotConnector::sendMessage(const char* message) {
	Serial3.write(message);
	Serial3.write(COMMAND_TERM);
}
