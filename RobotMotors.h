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
 */
class RobotMotors: public TaskInterface {
private:
	const uint8_t leftDriveID = 1; // motor shield drive # for the left drive
	const uint8_t rightDriveID = 2; // motor shield drive # for the right drive

	AF_DCMotor* leftMotor;
	AF_DCMotor* rightMotor;

	void runDrives(uint8_t speed, uint16_t duration,
	               uint8_t leftDirection, uint8_t rightDirection);

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
	 * A direct motor speed control method.
	 * @leftSpeed and @rightSpeed - actual speeds of the motors
	 * The speed is in range 000 - 511. 000 is full reverse, 511 means full ahead
	 * @duration - how long to turn in ms
	 */
	void setMotorSpeed(uint16_t leftSpeed, uint16_t rightSpeed, uint16_t duration);

	/*
	 * Stop both motors.
	 * This command can be called by the robot AI or other modules to change the drives state.
	 */
	void fullStop();
};

#endif /* ROBOTMOTORS_H_ */
