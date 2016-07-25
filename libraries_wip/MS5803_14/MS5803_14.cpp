/*	 MS5803_14
  	An Arduino library for the Measurement Specialties MS5803 family
  	of pressure sensors. This library uses I2C to communicate with the
  	MS5803 using the Wire library from Arduino.

 	This library only works with the MS5803-14BA model sensor. It DOES NOT
 	work with the other pressure-range models such as the MS5803-30BA or
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

#include "MS5803_14.h"
#include <Wire.h>

// For I2C, set the CSB Pin (pin 3) high for address 0x76, and pull low
// for address 0x77. If you use 0x77, change the value on the line below:
#define MS5803_I2C_ADDRESS    0x76 // or 0x77

#define CMD_RESET		0x1E	// ADC reset command
#define CMD_ADC_READ	0x00	// ADC read command
#define CMD_ADC_CONV	0x40	// ADC conversion command
#define CMD_ADC_D1		0x00	// ADC D1 conversion
#define CMD_ADC_D2		0x10	// ADC D2 conversion
#define CMD_ADC_256		0x00	// ADC resolution=256
#define CMD_ADC_512		0x02	// ADC resolution=512
#define CMD_ADC_1024	0x04	// ADC resolution=1024
#define CMD_ADC_2048	0x06	// ADC resolution=2048
#define CMD_ADC_4096	0x08	// ADC resolution=4096


// Create array to hold the 8 sensor calibration coefficients
static unsigned int      sensorCoeffs[8]; // unsigned 16-bit integer (0-65535)
// D1 and D2 need to be unsigned 32-bit integers (long 0-4294967295)
static uint32_t     D1 = 0;    // Store uncompensated pressure value
static uint32_t     D2 = 0;    // Store uncompensated temperature value
// These three variables are used for the conversion steps
// They should be signed 32-bit integer initially
// i.e. signed long from -2147483648 to 2147483647
static int32_t	dT = 0;
static int32_t 	TEMP = 0;
// These values need to be signed 64 bit integers
// (long long = int64_t)
static int64_t	Offset = 0;
static int64_t	Sensitivity  = 0;
static int64_t	T2 = 0;
static int64_t	OFF2 = 0;
static int64_t	Sens2 = 0;
// bytes to hold the results from I2C communications with the sensor
static byte HighByte;
static byte MidByte;
static byte LowByte;

// Some constants used in calculations below
const uint64_t POW_2_33 = 8589934592ULL; // 2^33 = 8589934592
const uint64_t POW_2_37 = 137438953472ULL; // 2^37 = 137438953472

//-------------------------------------------------
// Constructor
MS5803_14::MS5803_14( uint16_t Resolution )
{
	// The argument is the oversampling resolution, which may have values
	// of 256, 512, 1024, 2048, or 4096.
	_Resolution = Resolution;
}

//-------------------------------------------------
bool MS5803_14::Initialize()
{

	// Reset the sensor during startup
	resetSensor();

	int coeffsRead = 0;

	// Read sensor coefficients
	for( int i = 0; i < 8; i++ )
	{
		// The PROM starts at address 0xA0
		Wire.beginTransmission( MS5803_I2C_ADDRESS );
		Wire.write( 0xA0 + ( i * 2 ) );
		Wire.endTransmission();
		Wire.requestFrom( MS5803_I2C_ADDRESS, 2 );

		while( Wire.available() )
		{
			HighByte = Wire.read();
			LowByte = Wire.read();
			
			coeffsRead++;
		}

		sensorCoeffs[i] = ( ( ( unsigned int )HighByte << 8 ) + LowByte );
	}

	// Read always fails for Coefficient 7, so we can't perform a CRC. Skip this step for now.
	
	// // The last 4 bits of the 7th coefficient form a CRC error checking code.
	// unsigned char p_crc = sensorCoeffs[7];
	// // Use a function to calculate the CRC value
	// unsigned char n_crc = MS_5803_CRC( sensorCoeffs );

	// // If the CRC value doesn't match the sensor's CRC value, then the
	// // connection can't be trusted. Check your wiring.
	// if( p_crc != n_crc )
	// {
	// 	return false;
	// }

	// Use the number of coeff's successfully read as success check
	if( coeffsRead == 7 )
	{
		return true;
	}
	else
	{
		return false;
	}
}



//------------------------------------------------------------------
float MS5803_14::readSensor()
{
	// Choose from CMD_ADC_256, 512, 1024, 2048, 4096 for mbar resolutions
	// of 1, 0.6, 0.4, 0.3, 0.2 respectively. Higher resolutions take longer
	// to read.
	if( _Resolution == 256 )
	{
		D1 = MS5803_14_ADC( CMD_ADC_D1 + CMD_ADC_256 ); // read raw pressure
		D2 = MS5803_14_ADC( CMD_ADC_D2 + CMD_ADC_256 ); // read raw temperature
	}
	else if( _Resolution == 512 )
	{
		D1 = MS5803_14_ADC( CMD_ADC_D1 + CMD_ADC_512 ); // read raw pressure
		D2 = MS5803_14_ADC( CMD_ADC_D2 + CMD_ADC_512 ); // read raw temperature
	}
	else if( _Resolution == 1024 )
	{
		D1 = MS5803_14_ADC( CMD_ADC_D1 + CMD_ADC_1024 ); // read raw pressure
		D2 = MS5803_14_ADC( CMD_ADC_D2 + CMD_ADC_1024 ); // read raw temperature
	}
	else if( _Resolution == 2048 )
	{
		D1 = MS5803_14_ADC( CMD_ADC_D1 + CMD_ADC_2048 ); // read raw pressure
		D2 = MS5803_14_ADC( CMD_ADC_D2 + CMD_ADC_2048 ); // read raw temperature
	}
	else if( _Resolution == 4096 )
	{
		D1 = MS5803_14_ADC( CMD_ADC_D1 + CMD_ADC_4096 ); // read raw pressure
		D2 = MS5803_14_ADC( CMD_ADC_D2 + CMD_ADC_4096 ); // read raw temperature
	}

	// Calculate 1st order temperature, dT is a long integer
	// D2 is originally cast as an uint32_t, but can fit in a int32_t, so we'll
	// cast both parts of the equation below as signed values so that we can
	// get a negative answer if needed
	dT = ( int32_t )D2 - ( ( int32_t )sensorCoeffs[5] * 256 );
	// Use integer division to calculate TEMP. It is necessary to cast
	// one of the operands as a signed 64-bit integer (int64_t) so there's no
	// rollover issues in the numerator.
	TEMP = 2000 + ( ( int64_t )dT * sensorCoeffs[6] ) / 8388608LL;
	// Recast TEMP as a signed 32-bit integer
	TEMP = ( int32_t )TEMP;


	// All operations from here down are done as integer math until we make
	// the final calculation of pressure in mbar.


	// Do 2nd order temperature compensation (see pg 9 of MS5803 data sheet)
	// I have tried to insert the fixed values wherever possible
	// (i.e. 2^31 is hard coded as 2147483648).
	if( TEMP < 2000 )
	{
		// For 14 bar model // If temperature is below 20.0C
		T2 = 3 * ( ( int64_t )dT * dT ) / POW_2_33 ;
		T2 = ( int32_t )T2; // recast as signed 32bit integer
		OFF2 = 3 * ( ( TEMP - 2000 ) * ( TEMP - 2000 ) ) / 2 ;
		Sens2 = 5 * ( ( TEMP - 2000 ) * ( TEMP - 2000 ) ) / 8 ;
	}
	else     // if TEMP is > 2000 (20.0C)
	{
		// For 14 bar model
		T2 = 7 * ( ( uint64_t )dT * dT ) / POW_2_37;
		T2 = ( int32_t )T2; // recast as signed 32bit integer
		OFF2 = 1 * ( ( TEMP - 2000 ) * ( TEMP - 2000 ) ) / 16;
		Sens2 = 0;
	}

	// Additional compensation for very low temperatures (< -15C)
	if( TEMP < -1500 )
	{
		// For 14 bar model
		OFF2 = OFF2 + 7 * ( ( TEMP + 1500 ) * ( TEMP + 1500 ) );
		Sens2 = Sens2 + 4 * ( ( TEMP + 1500 ) * ( TEMP + 1500 ) );
	}

	// Calculate initial Offset and Sensitivity
	// Notice lots of casts to int64_t to ensure that the
	// multiplication operations don't overflow the original 16 bit and 32 bit
	// integers
	// For 14 bar sensor
	Offset = ( int64_t )sensorCoeffs[2] * 65536 + ( sensorCoeffs[4] * ( int64_t )dT ) / 128;
	Sensitivity = ( int64_t )sensorCoeffs[1] * 32768 + ( sensorCoeffs[3] * ( int64_t )dT ) / 256;

	// Adjust TEMP, Offset, Sensitivity values based on the 2nd order
	// temperature correction above.
	TEMP = TEMP - T2; // both should be int32_t
	Offset = Offset - OFF2; // both should be int64_t
	Sensitivity = Sensitivity - Sens2; // both should be int64_t

	// Final compensated pressure calculation. We first calculate the pressure
	// as a signed 32-bit integer (mbarInt), then convert that value to a
	// float (mbar).
	// For 14 bar sensor
	mbarInt = ( ( D1 * Sensitivity ) / 2097152 - Offset ) / 32768;
	mbar = ( float )mbarInt / 10;
	
	// Calculate the human-readable temperature in Celsius
	tempC  = ( float )TEMP / 100;
	
	return mbar;
}

//------------------------------------------------------------------
// Function to check the CRC value provided by the sensor against the
// calculated CRC value from the rest of the coefficients.
// Based on code from Measurement Specialties application note AN520
// http://www.meas-spec.com/downloads/C-Code_Example_for_MS56xx,_MS57xx_%28except_analog_sensor%29_and_MS58xx_Series_Pressure_Sensors.pdf
unsigned char MS5803_14::MS5803_14_CRC( unsigned int n_prom[] )
{
	int cnt;				// simple counter
	unsigned int n_rem;		// crc reminder
	unsigned int crc_read;	// original value of the CRC
	unsigned char  n_bit;
	n_rem = 0x00;
	crc_read = sensorCoeffs[7];		// save read CRC
	sensorCoeffs[7] = ( 0xFF00 & ( sensorCoeffs[7] ) ); // CRC byte replaced with 0

	for( cnt = 0; cnt < 16; cnt++ )
	{
		// choose LSB or MSB
		if( cnt % 2 == 1 )
		{
			n_rem ^= ( unsigned short )( ( sensorCoeffs[cnt >> 1] ) & 0x00FF );
		}
		else
		{
			n_rem ^= ( unsigned short )( sensorCoeffs[cnt >> 1] >> 8 );
		}

		for( n_bit = 8; n_bit > 0; n_bit-- )
		{
			if( n_rem & ( 0x8000 ) )
			{
				n_rem = ( n_rem << 1 ) ^ 0x3000;
			}
			else
			{
				n_rem = ( n_rem << 1 );
			}
		}
	}

	n_rem = ( 0x000F & ( n_rem >> 12 ) ); // // final 4-bit reminder is CRC code
	sensorCoeffs[7] = crc_read; // restore the crc_read to its original place
	// Return n_rem so it can be compared to the sensor's CRC value
	return ( n_rem ^ 0x00 );
}

//-----------------------------------------------------------------
// Send commands and read the temperature and pressure from the sensor
unsigned long MS5803_14::MS5803_14_ADC( char commandADC )
{
	// D1 and D2 will come back as 24-bit values, and so they must be stored in
	// a long integer on 8-bit Arduinos.
	long result = 0;
	
	// Send the command to do the ADC conversion on the chip
	Wire.beginTransmission( MS5803_I2C_ADDRESS );
	Wire.write( CMD_ADC_CONV + commandADC );
	Wire.endTransmission();

	// Wait a specified period of time for the ADC conversion to happen
	// See table on page 1 of the MS5803 data sheet showing response times of
	// 0.5, 1.1, 2.1, 4.1, 8.22 ms for each accuracy level.
	switch( commandADC & 0x0F )
	{
		case CMD_ADC_256 :
			delay( 1 ); // 1 ms
			break;

		case CMD_ADC_512 :
			delay( 3 ); // 3 ms
			break;

		case CMD_ADC_1024:
			delay( 4 );
			break;

		case CMD_ADC_2048:
			delay( 6 );
			break;

		case CMD_ADC_4096:
			delay( 10 );
			break;
	}

	// Now send the read command to the MS5803
	Wire.beginTransmission( MS5803_I2C_ADDRESS );
	Wire.write( ( byte )CMD_ADC_READ );
	Wire.endTransmission();
	
	// Then request the results. This should be a 24-bit result (3 bytes)
	Wire.requestFrom( MS5803_I2C_ADDRESS, 3 );

	while( Wire.available() )
	{
		HighByte = Wire.read();
		MidByte = Wire.read();
		LowByte = Wire.read();
	}

	// Combine the bytes into one integer
	result = ( ( long )HighByte << 16 ) + ( ( long )MidByte << 8 ) + ( long )LowByte;
	return result;
}

//----------------------------------------------------------------
// Sends a power on reset command to the sensor.
void MS5803_14::resetSensor()
{
	Wire.beginTransmission( MS5803_I2C_ADDRESS );
	Wire.write( CMD_RESET );
	Wire.endTransmission();
	delay( 10 );
}
