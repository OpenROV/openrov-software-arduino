#pragma once

#include <Arduino.h>
#include "SysConfig.h"
#if CONTROLLERBOARD == CONTROLLERBOARD_CAPE

// Pin definitions
// See your core's variant.cpp for full pinmuxing configuration for this board

// GPIO pins
// #define PIN_STANDARD_LIGHTS		35
// #define PIN_CAMERA_MOUNT		59
// #define PIN_PORT_MOTOR			23
// #define PIN_VERTICAL_MOTOR		24
// #define PIN_STARBOARD_MOTOR		25
// #define PIN_ALTSERVO			60
// #define PIN_ENABLE_ESC			42
// #define PIN_ENABLE_EXT_I2C		50
// #define PIN_ENABLE_INT_I2C  	51

// // PWM pins
// #define PIN_EXTERNAL_LIGHTS		45
// #define PIN_LASERS          	36

// // Analog pins
// #define PIN_BATT1_V  			PIN_A0		// 
// #define PIN_BATT2_V  			PIN_A1 		// 
// #define PIN_NON_ESC_I  			PIN_A2		// 
// #define PIN_ESC3_I  			PIN_A3 		// 
// #define PIN_BATT_TUBE1_I  		PIN_A8 		// 
// #define PIN_BATT_TUBE2_I  		PIN_A9 		// 
// #define PIN_HUMIDITY 			PIN_A10		// 
// #define PIN_BOARD_TEMP 			PIN_A11 	// 
// #define PIN_EXT_LOAD_I 			PIN_A12 	// 
// #define PIN_ESC1_I 				PIN_A13 	// 
// #define PIN_BATT_BUS_V 			PIN_A14 	// 
// #define PIN_ESC2_I 				PIN_A15 	// 

void SetPinModes()
{
    
    // // Setup digital IO pins in OUTPUT mode
    // pinMode( PIN_LED_0,         OUTPUT );
    // pinMode( PIN_LED_1,         OUTPUT );
    // pinMode( PIN_EN_INTI2C,     OUTPUT );
    // pinMode( PIN_EN_EXTI2C,     OUTPUT );
    // pinMode( PIN_EN_ESC,        OUTPUT );
    // pinMode( PIN_ESC_PRECHARGE, OUTPUT );
    
    // pinMode( PIN_EN_PROGRAM,    INPUT );
    
    // pinMode( 23, OUTPUT );
    // pinMode( 24, OUTPUT );
    // pinMode( 25, OUTPUT );
    // pinMode( 26, OUTPUT );
    // pinMode( 37, OUTPUT );
    // pinMode( 38, OUTPUT );
}

#endif