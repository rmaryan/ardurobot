# ArduRobot

This is a source code for the Arduino-based robot with simple cooperative multitasking system and isolated modules which control different functions.

See the details in our blogs:
 * http://onerobotstory.blogspot.com/ (English)
 * http://refunpro.blogspot.com/ (Ukrainian)
 
## Hardware

* [Arduino Mega 2560](http://www.banggood.com/Mega2560-R3-ATmega2560-16AU-Control-Board-With-USB-Cable-For-Arduino-p-73020.html?p=M908156347868201609Y)
* [Motor Driver Shield V1](http://www.banggood.com/Motor-Drive-Shield-L293D-for-Arduino-Duemilanove-Mega-UNO-p-72855.html?p=M908156347868201609Y)
* [ESP13 Shield](http://www.banggood.com/ESP8266-Web-Server-Port-WiFi-Expansion-Board-ESP-13-Compatible-With-Arduino-p-1008124.html?p=M908156347868201609Y)
* [Ultrasonic distance sensor HC-SR04 on a cradle](http://www.banggood.com/HC-SR04-Ultrasonic-Module-Distance-Measuring-Transducer-Sensor-With-Mount-Bracket-p-1029445.html?p=M908156347868201609Y)
* [TowerPro SG90 Mini Gear Micro Servo](http://www.banggood.com/TowerPro-SG90-Mini-Gear-Micro-Servo-9g-For-RC-Airplane-Helicopter-p-1009914.html?p=M908156347868201609Y) to turn the ultrasonic sensor left and right
* [Infrared Obstacle Avoiding Sensors](http://www.banggood.com/5Pcs-Infrared-Obstacle-Avoidance-Sensor-For-Arduino-Smart-Car-Robot-p-951032.html?p=M908156347868201609Y)
* Tank chassis ([Geekcreit T300](http://www.banggood.com/DIY-DT300-WIFI-Double-Layer-Decker-Tracked-Model-Tank-Compatible-With-Arduino-UNO-R3-p-1079947.html?p=M908156347868201609Y) - can be any other)
* Rechargeable Battery 6xAA 2500 mAh

## Software

### Core

*robot.ino* - the entry point with the standard Arduino setup() and loop() functions

*TaskInterface.h* - defines the parent class for all "tasks" which are given a quant of time in the main multitasking loop

*RobotAI.h/.cpp* - the core class which orchestrates all the robot actions (including scenarios execution and remote control over Wi-Fi)

### Robot modules

*RobotDistanceSensor.h/.cpp* - class for accessing the ultrasonic and infrared distance sensor

*RobotConnector.h/.cpp* - class for managing the remote connection through the serial interface

*RobotLights.h/.cpp* - class for controlling robot LED's

*RobotMotors.h/.cpp* - class for controling chasis motors

*RobotVoice.h/.cpp* - class for generating sounds

## Remote Control Protocol

Remote control is performed over Wi-Fi connection using simple telnet-like connection. It is also compatible with RoboRemo: http://www.roboremo.com/

| Command From RC | Action                 | Response from the Robot (on success)        |
| ----------------| ---------------------- | ------------------------------------------- |
| MI              | Mode-Idle              | MI                                          |
| MA              | Mode-AI                | MA                                          |
| MS              | Mode-Scenario          | MS                                          |
| MR              | Mode-Remote Control    | MR                                          |
| W               | Move Forward           | -                                           | 
| S               | Move Backward          | -                                           |
| A               | Turn Left              | -                                           |
| D               | Turn Right             | -                                           |
| LF              | Toggle Front Lights    | LF1 or LF0                                  |
| LR              | Toggle Rear Lights     | LR1 or LR0                                  |
| LS              | Toggle Side Lights     | LS1 or LS0                                  |
| R0              | Refresh Distances (with the head turn) | RLxx - left distance, where xx is a distance in centimeters with the leading zeros. RL-- means "not available" |
|                 |                        | RFxx - front distance                          |
|                 |                        | RRxx - right distance                          |
|                 |                        | ROABCD - obstacle detectors state ("1"s or "0"s), <BR> A - left-ahead, B - left-edge, C - right-edge, D - right-ahead |
| R1              | Refresh Distances (without the head turn) | same as above                |
| XAAABBB         | Set the drives speed to AAA (left) and BBB (right). The speed is in range 000 - 511. 000 is full reverse, 511 means full ahead. | - |
| -               | Debug message from the robot | ~message text                             |

All commands should be followed by \n.

## Changes List
### 1.4
Added support for the advanced remote control based on the RoboRemote desktop RC dashboard project https://github.com/rmaryan/roboremote. More details here: http://refunpro.blogspot.com/2017/07/17.html (Ukr) 
### 1.3
Added support for the Infrared Obstacle Detection Sensors. More details here: http://refunpro.blogspot.com/2017/03/16.html (Ukr)
### 1.2
Added support for the Ultrasonic Distance Sensor. More details here: http://onerobotstory.blogspot.com/2017/07/10-ultrasonic-distance-sensor.html (Eng) or here: http://refunpro.blogspot.com/2017/01/15.html (Ukr)
### 1.1
This is the initial version which includes simple scenario and the remote control code. More details here: http://onerobotstory.blogspot.com/2017/06/9-wifi-remote-control.html (Eng) or here: https://refunpro.blogspot.com/2016/11/10.html (Ukr)
