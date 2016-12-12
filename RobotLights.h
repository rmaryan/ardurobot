/*
 * RobotLights.h
 *
 *  Created on: 12 זמגע. 2016 נ.
 *      Author: snm
 */

#ifndef ROBOTLIGHTS_H_
#define ROBOTLIGHTS_H_

#include "TaskInterface.h"

class RobotLights: public TaskInterface {
private:
	uint8_t dataPin;
	uint8_t syncPin;
	uint8_t latchPin;
public:
	/*
	 * dataPin - the pin used to feed data to the shift register
	 * syncPin - the pin to send the synchronization signal to the register
	 * latchPin - the pin to refresh the register outputs state
	 */
	RobotLights(uint8_t in_dataPin, uint8_t in_syncPin, uint8_t in_latchPin);
	virtual ~RobotLights();
	virtual void processTask();
};

#endif /* ROBOTLIGHTS_H_ */
