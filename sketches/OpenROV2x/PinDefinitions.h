#pragma once

#include <Arduino.h>
#include "SysConfig.h"

// Pin definitions
// ------------------------------
// See your core's variant.cpp for full pinmuxing configuration for this board
// ------------------------------

// ------------------------------
// Pin assignments
// ------------------------------

// Board LEDs
#define PIN_LED_0                   13
#define PIN_LED_1                   49

// Servo Pins (also digital outputs)
#define PIN_SERVO_1                 6
#define PIN_SERVO_2                 7
#define PIN_SERVO_3                 8
#define PIN_SERVO_4                 9
#define PIN_SERVO_5                 10
#define PIN_SERVO_6                 11
    
// PWM Pins
#define PIN_PWM_1                   44
#define PIN_PWM_2                   45
#define PIN_PWM_3                   46
#define PIN_PWM_4                   12

// GPIO
#define PIN_ENABLE_ESC              16
#define PIN_ENABLE_I2C          	48

// Standard Motor pin setup
#define PIN_PORT_MOTOR			    PIN_SERVO_1
#define PIN_STARBOARD_MOTOR		    PIN_SERVO_3
#define PIN_VERTICAL_MOTOR		    PIN_SERVO_2
// Not used						    PIN_SERVO_4
// Not used						    PIN_SERVO_5
// Not used							PIN_SERVO_6

// 2xV2 alternative pin setup
// PIN_PORT_MOTOR					PIN_SERVO_1
// PIN_STARBOARD_MOTOR			    PIN_SERVO_3
#define PIN_PORT_VERTICAL_MOTOR		PIN_SERVO_2
#define PIN_STAR_VERTICAL_MOTOR		PIN_SERVO_4
// Not used						    PIN_SERVO_5
// Not used						    PIN_SERVO_6

// V2x1xV2 alternative pin setup
#define PIN_PORT_FORWARD_MOTOR		PIN_SERVO_4
#define PIN_STAR_FORWARD_MOTOR		PIN_SERVO_5
// PIN_VERTICAL_MOTOR				PIN_SERVO_2
#define PIN_PORT_AFT_MOTOR			PIN_SERVO_1
#define PIN_STAR_AFT_MOTOR			PIN_SERVO_3
// Not used						    PIN_SERVO_6

// Other Servos
#define PIN_CAMERA_MOUNT		    PIN_SERVO_6
#define PIN_ALTSERVO			    PIN_SERVO_4

// -----------
// PWM pins

#define PIN_STANDARD_LIGHTS		    PIN_PWM_1
#define PIN_LASERS                  PIN_PWM_2
// Not yet used                     PIN_PWM_3
// Not yet used		    			PIN_PWM_4

// -----------
// Analog pins

#define PIN_BOARD_TEMP 		    	A8		// 
#define PIN_BOARD_I  		    	A0		// 
#define PIN_ESC1_I 			    	A3 		// 
#define PIN_ESC2_I 			    	A2 		// 
#define PIN_ESC3_I  		    	A1 		// 
#define PIN_BATT_TUBE1_I  	    	A6 		// 
#define PIN_BATT_TUBE2_I  	    	A5 		// 
#define PIN_BOARD_V 		    	A4 		//

// Other Defines
#define HAS_ESC_POWER_SWITCH		1
