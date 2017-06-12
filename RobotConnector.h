/*
 * RobotConnector.h
 *
 */

#ifndef ROBOTCONNECTOR_H_
#define ROBOTCONNECTOR_H_

#include "TaskInterface.h"

/*
 * This class hides all the details, related to receiving
 * the Remote Control Commands and replying to them.
 * No buffering is performed here - we assume ES8266 board
 * takes care about this.
 */
class RobotConnector: public TaskInterface {
private:
	static const uint8_t MAX_MSG_SIZE = 200;
	char messageBuffer[MAX_MSG_SIZE];
	uint8_t messageSize = 0;
	bool messageReady = false;
	static const char COMMAND_TERM = '\n';
public:
	RobotConnector();

	/*
	 * This function is called by a task scheduler each time the world ticks.
	 * Here the bytes are read from the serial stream and tied to a command strings.
	 */
	virtual void processTask();

	/*
	 * Returns true if some command was received and is available for processing
	 */
	virtual bool commandAvailable();

	/*
	 * Returns the last received command
	 */
	virtual char* getCommand();

	/*
	 * Sends a message to the RC client.
	 * The message is read till the terminating 0.
	 * 0 is not sent.
	 * Message bytes are followed by the COMMAND_TERM (\n)
	 */
	virtual void sendMessage(const char* message);
};

#endif /* ROBOTCONNECTOR_H_ */
