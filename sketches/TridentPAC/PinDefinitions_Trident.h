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
#define PIN_D0                      26u
#define PIN_D1                      28u
#define PIN_D2                      31u
#define PIN_D3                      32u
#define PIN_D4                     	37u
#define PIN_D5                     	38u
#define PIN_D6                     	39u
#define PIN_D7                     	40u
#define PIN_D8                     	41u
#define PIN_D9                     	42u
#define PIN_D10                     46u
#define PIN_D11                     49u
#define PIN_D12                     50u
#define PIN_D13                     51u

// Servo Pins (also digital outputs)
#define PIN_SERVO_0                 23u 
#define PIN_SERVO_1                 24u
#define PIN_SERVO_2                 25u
    
// PWM Pins
#define PIN_PWM_0                   45u

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
#define PIN_A18   		    	    19u
#define PIN_A19   		    	    20u

// ------------------------------
// Pin assignments
// ------------------------------

// -----------
// GPIO
#define PIN_ENABLE_ESC              PIN_D9
#define PIN_ENABLE_I2C          	PIN_D13

// -----------
// Servos

// Standard Motor pin setup
#define PIN_PORT_MOTOR			    PIN_SERVO_0
#define PIN_VERTICAL_MOTOR		    PIN_SERVO_1
#define PIN_STARBOARD_MOTOR		    PIN_SERVO_2

// -----------
// PWM pins

#define PIN_STANDARD_LIGHTS		    PIN_PWM_0

// -----------
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

#define SerialDebug					Serial1
#define SerialMotor0				Serial3
#define SerialMotor1				Serial2
#define SerialMotor2				Serial4

#endif