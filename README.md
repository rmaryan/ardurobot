# ArduRobot

This is a source code for the Arduino-based robot with simple cooperative multitasking system and isolated modules which control different functions.

See the details at http://refunpro.blogspot.com/ (in Ukrainian)

## Hardware

* Arduino Mega 2560
* Motor Driver Shield V1
* ESP13 Shield
* Ultrasonic distance sensor HC-SR04
* Tank chasis ([Geekcreit T300](http://www.banggood.com/DIY-DT300-WIFI-Double-Layer-Decker-Tracked-Model-Tank-Compatible-With-Arduino-UNO-R3-p-1079947.html?p=M908156347868201609Y) - can be any other)
* Rechargeable Battery 6xAA 2500 mAh

## Software

### Core

*robot.ino* - the entry point with the standard Arduino setup() and loop() functions

*TaskInterface.h* - defines the parent class for all "tasks" which are given a quant of time in the main multitasking loop

*RobotAI.h/.cpp* - the core class which orchestrates all the robot actions (including scenarios execution and remote control over Wi-Fi)

### Robot modules

*RobotDistanceSensor.h/.cpp* - class for accessing the ultrasonic distance sensor

*RobotLights.h/.cpp* - class for controlling robot LED's

*RobotMotors.h/.cpp* - class for controling chasis motors

*RobotVoice.h/.cpp* - class for generating sounds
