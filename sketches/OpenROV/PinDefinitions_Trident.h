#pragma once

#include <Arduino.h>
#include "SysConfig.h"
#if CONTROLLERBOARD == CONTROLLERBOARD_TRIDENT

// ----------------------------
// Pin Definitions
// See your core's variant.cpp for full pinmuxing configuration for this board
// ----------------------------

// Board LEDs
#define PIN_LED_0                   31u
#define PIN_LED_1                   28u

// GPIO pins
#define PIN_D0                      42u
#define PIN_D1                      50u
#define PIN_D2                      51u
#define PIN_D3                      32u
#define PIN_D4                     	49u

// Servo Pins (also digital outputs)
#define PIN_SERVO_1                 23u 
#define PIN_SERVO_2                 24u
#define PIN_SERVO_3                 25u
#define PIN_SERVO_4                 26u
#define PIN_SERVO_5                 37u
#define PIN_SERVO_6                 38u
#define PIN_SERVO_7                 59u
#define PIN_SERVO_8                 60u
    
// PWM Pins
#define PIN_PWM_1                   35u
#define PIN_PWM_2                   36u
#define PIN_PWM_3                   45u
#define PIN_PWM_4                   46u
#define PIN_PWM_5                   19u
#define PIN_PWM_6                   20u

// Analog Pins
#define PIN_A0                      3u
#define PIN_A1                      4u
#define PIN_A2                      11u
#define PIN_A3  			        12u
#define PIN_A8   		        	61u
#define PIN_A9   		        	62u
#define PIN_A10  			        63u
#define PIN_A11 		        	64u
#define PIN_A12 		    	    5u
#define PIN_A13   		    	    6u
#define PIN_A14   		    	    9u
#define PIN_A15   		    	    10u

// ------------------------------
// Pin assignments
// ------------------------------

// GPIO
#define PIN_ENABLE_ESC              PIN_D0
#define PIN_ENABLE_EXT_I2C          PIN_D1
#define PIN_ENABLE_INT_I2C          PIN_D2
#define PIN_ENABLE_PROGRAM          PIN_D3
#define PIN_ENABLE_ESC_PRECHARGE    PIN_D4

// Servos
#define PIN_PORT_MOTOR			    PIN_SERVO_1
#define PIN_STARBOARD_MOTOR		    PIN_SERVO_2
#define PIN_VERTICAL_MOTOR		    PIN_SERVO_3
// Not used		                    PIN_SERVO_4
// Not used		                    PIN_SERVO_5
// Not used		                    PIN_SERVO_6
#define PIN_CAMERA_MOUNT		    PIN_SERVO_7
#define PIN_ALTSERVO			    PIN_SERVO_8

// PWM pins
#define PIN_STANDARD_LIGHTS		    PIN_PWM_1
// Not yet used                     PIN_PWM_2
#define PIN_EXTERNAL_LIGHTS		    PIN_PWM_3
#define PIN_LASERS                  PIN_PWM_4
// Not yet used                     PIN_PWM_5
// Not yet used                     PIN_PWM_6


// Analog pins (? = todo)
#define PIN_BATT1_V  		    	PIN_A0		// ?
#define PIN_BATT2_V  		    	PIN_A1 		// ?
#define PIN_NON_ESC_I  		    	PIN_A2		// 0-3.3A
#define PIN_ESC3_I  		    	PIN_A3 		// 0-11A
#define PIN_BATT_TUBE1_I  	    	PIN_A8 		// 0-11A
#define PIN_BATT_TUBE2_I  	    	PIN_A9 		// 0-11A
#define PIN_HUMIDITY 		    	PIN_A10		// ?
#define PIN_BOARD_TEMP 		    	PIN_A11 	// ?
#define PIN_EXT_LOAD_I 		    	PIN_A12 	// ?
#define PIN_ESC1_I 			    	PIN_A13 	// 0-11A
#define PIN_BATT_BUS_V 		    	PIN_A14 	// ?
#define PIN_ESC2_I 			    	PIN_A15 	// 0-11A

// Other Defines
#define HAS_ESC_POWER_SWITCH		1

#endif