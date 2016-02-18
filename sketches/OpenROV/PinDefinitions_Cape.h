#pragma once

#include <Arduino.h>
#include "SysConfig.h"
#if CONTROLLERBOARD == CONTROLLERBOARD_CAPE

// Pin definitions
// ------------------------------
// See your core's variant.cpp for full pinmuxing configuration for this board
// ------------------------------


// ------------------------------
// Pin assignments
// ------------------------------

// Board LEDs
#define PIN_LED_0                   13
#define PIN_LED_1                   2

// Servo Pins (also digital outputs)
#define PIN_SERVO_1                 9
#define PIN_SERVO_2                 10
#define PIN_SERVO_3                 11
#define PIN_SERVO_4                 3
    
// PWM Pins
#define PIN_PWM_1                   5
#define PIN_PWM_2                   6

// Standard Motor pin setup
#define PIN_PORT_MOTOR			    PIN_SERVO_1
#define PIN_VERTICAL_MOTOR		    PIN_SERVO_2
#define PIN_STARBOARD_MOTOR		    PIN_SERVO_3

// Other Servos
#define PIN_CAMERA_MOUNT		    PIN_SERVO_4

// -----------
// PWM pins

#define PIN_STANDARD_LIGHTS		    PIN_PWM_1
#define PIN_LASERS                  PIN_PWM_2

// -----------
// Analog pins
#define PIN_BOARD_V 		    	A0 		//
#define PIN_BOARD_I  		    	A3		// 

#endif