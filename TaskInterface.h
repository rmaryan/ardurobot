/*
 * TaskInterface.h
 *
 */

#ifndef TASKINTERFACE_H_
#define TASKINTERFACE_H_

#include<Arduino.h>

/*
 * The parent class for all tasks which can be processed by the task scheduler
 */
class TaskInterface {
protected:
	/*
	 * A variable which stores the time when the next event should occur
	 */
	unsigned long taskDeadline = 0;

	/*
	 *  Just a common method which can be helpful to all descendants in scheduling their next action.
	 * Sets a task deadline to happen after some time period (duration)
	 */
	void scheduleTimedTask(uint16_t duration) {
		taskDeadline = millis() + duration;
	}

	/*
	 * A common method which checks if the scheduled deadline was already reached
	 */
	bool reachedDeadline() {
		if(taskDeadline==0) {
			return false;
		} else {
			return taskDeadline < millis();
		}
	}
public:
	virtual ~TaskInterface() {};

	/*
	 * A method to be overridden to do something useful when the world timer ticks
	 */
	virtual void processTask() = 0;
};

#endif /* TASKINTERFACE_H_ */
