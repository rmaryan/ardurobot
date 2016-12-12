/*
 * RobotMotors.h
 *
 */

#ifndef ROBOTMOTORS_H_
#define ROBOTMOTORS_H_

#include "TaskInterface.h"

/*
 * Codes to store the motors state
 */
enum MotorCurrentCommand { cmdFwd, cmdRight, cmdLeft, cmdStop};


/*
 * The motors control class.
 * This version assumes managing motors through the transistor-based schematics.
 * No reverse gear.
 */

class RobotMotors: public TaskInterface {
private:
	uint8_t leftDrivePin = 0; // pin which controls the speed of the left drive
	uint8_t rightDrivePin = 0; // ping which controls the speed of the right drive
	MotorCurrentCommand currentCommand = cmdStop; // the field where current motors state is stored

public:
	/*
	 * Constructor initializes the motors and stores locally the pin numbers whihc control the motors.
	 */
	RobotMotors(uint8_t in_leftDrivePin, uint8_t in_rightDrivePin);

	/*
	 * Destructor makes sure the motors are finally stopped
	 */
	virtual ~RobotMotors();

	/*
	 * This function is called by a task scheduler each time the world ticks.
	 * The motors state change logic is located here.
	 */
	virtual void processTask();

	/*
	 * Drive forward for the duration specified. If zero - drive without limitations.
	 * This command can be called by the robot AI or other modules to change the drives state.
	 * @speed - the relative speed from 0 to 255
	 * @duration - how long to drive in ms
	 */
	void driveForward(uint8_t speed = 255, uint16_t duration = 0);

	/*
	 * Turns right
	 * This command can be called by the robot AI or other modules to change the drives state.
	 * @speed - the relative speed from 0 to 255
	 * @duration - how long to turn in ms
	 */
	void turnRight(uint8_t speed = 255, uint16_t duration = 0);

	/*
	 * Turns left.
	 * This command can be called by the robot AI or other modules to change the drives state.
	 * @speed - the relative speed from 0 to 255
	 * @duration - how long to turn in ms
	 */
	void turnLeft(uint8_t speed = 255, uint16_t duration = 0);

	/*
	 * Stop both motors.
	 * This command can be called by the robot AI or other modules to change the drives state.
	 */
	void fullStop();
};

#endif /* ROBOTMOTORS_H_ */
