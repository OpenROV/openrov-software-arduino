/*	 MS5803_30
  	An Arduino library for the Measurement Specialties MS5803 family
  	of pressure sensors. This library uses I2C to communicate with the
  	MS5803 using the Wire library from Arduino.

 	This library only works with the MS5803-30BA model sensor. It DOES NOT
 	work with the other pressure-range models such as the MS5803-14BA or
 	MS5803-01BA. Those models will return incorrect pressure and temperature
 	readings if used with this library. See http://github.com/millerlp for
 	libraries for the other models.

  	No warranty is given or implied. You are responsible for verifying that
 	the outputs are correct for your sensor. There are likely bugs in
 	this code that could result in incorrect pressure readings, particularly
 	due to variable overflows within some pressure ranges.
  	DO NOT use this code in a situation that could result in harm to you or
  	others because of incorrect pressure readings.


  	Licensed under the GPL v3 license.
  	Please see accompanying LICENSE.md file for details on reuse and
  	redistribution.

  	Copyright Luke Miller, April 1 2014
*/


#pragma once

#include <Arduino.h>

class MS5803_30
{
public:
	// Constructor for the class.
	// The argument is the desired oversampling resolution, which has
	// values of 256, 512, 1024, 2048, 4096
	MS5803_30( uint16_t Resolution = 512 );

	// Initialize the sensor
	bool Initialize();

	void resetSensor();
	float readSensor();

	//*********************************************************************
	// Additional methods to extract temperature, pressure (mbar), and the
	// D1,D2 values after readSensor() has been called

	// Return temperature in degrees Celsius.
	float temperature() const
	{
		return tempC;
	}

	// Return pressure in mbar.
	float pressure() const
	{
		return mbar;
	}

	// Return the D1 and D2 values, mostly for troubleshooting
	unsigned long D1val() const
	{
		return D1;
	}
	unsigned long D2val() const
	{
		return D2;
	}

private:

	float mbar; // Store pressure in mbar.
	float tempC; // Store temperature in degrees Celsius

	unsigned long D1;	// Store D1 value
	unsigned long D2;	// Store D2 value
	int32_t mbarInt; // pressure in mbar, initially as a signed long integer

	// Check data integrity with CRC4
	unsigned char MS5803_30_CRC( unsigned int n_prom[] );

	// Handles commands to the sensor.
	unsigned long MS5803_30_ADC( char commandADC );

	// Oversampling resolution
	uint16_t _Resolution;
};