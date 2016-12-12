/*
 * RobotMotors.h
 *
 */

#ifndef ROBOTMOTORS_H_
#define ROBOTMOTORS_H_

#include "TaskInterface.h"
#include "AFMotor.h"

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
	const uint8_t leftDriveID = 1; // motor shield drive # for the left drive
	const uint8_t rightDriveID = 2; // motor shield drive # for the right drive

	AF_DCMotor* leftMotor;
	AF_DCMotor* rightMotor;

	MotorCurrentCommand currentCommand = cmdStop; // the field where current motors state is stored

public:
	/*
	 * Constructor initializes the motors and stores locally the pin numbers whihc control the motors.
	 */
	RobotMotors();

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
	 * Drive forward for the duration specified.
	 * This command can be called by the robot AI or other modules to change the drives state.
	 * @speed - the relative speed from 0 to 255
	 * @duration - how long to drive in ms. If zero - drive without limitations.
	 */
	void driveForward(uint8_t speed = 255, uint16_t duration = 0);

	/*
	 * Drive backward for the duration specified.
	 * This command can be called by the robot AI or other modules to change the drives state.
	 * @speed - the relative speed from 0 to 255
	 * @duration - how long to drive in ms. If zero - drive without limitations.
	 */
	void driveBackward(uint8_t speed = 255, uint16_t duration = 0);

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
