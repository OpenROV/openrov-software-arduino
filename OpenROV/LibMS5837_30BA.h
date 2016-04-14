#pragma once

// Includes
#include <Arduino.h>

// Defines
#define MS5837_RES_256          256u
#define MS5837_RES_512          512u
#define MS5837_RES_1024         1024u
#define MS5837_RES_2048         2048u
#define MS5837_RES_4096         4096u
#define MS5837_RES_8192         8192u

#define MS5837_WATERTYPE_FRESH  0
#define MS5837_WATERTYPE_SALT   1

class MS5837_30BA
{
public:
   
    // Attributes
    float       m_depth;
	float       m_depthOffset;

	bool        m_hasValidCoefficients;

	 // Methods
	MS5837_30BA( uint16_t resolutionIn = MS5837_RES_4096 );
	
	int Initialize();
	int GetCalibrationCoefficients();
	int Reset();
	int StartPressureConversion();
	int StartTemperatureConversion();
	int Read();
	
	void SetWaterType( int waterTypeIn );

private:

    // Attributes
	float m_pressure_mbar;
	float m_temperature_c;
	
	// Create array to hold the 8 sensor calibration coefficients
    uint16_t m_sensorCoeffs[8]; // unsigned 16-bit integer (0-65535)
    
    uint32_t D1 = 0;        // Digital pressure value
    uint32_t D2 = 0;        // Digital temperature value
    
    int32_t dT = 0;         // Difference between actual and reference temperature
    int32_t TEMP = 0;       // Actual temperature
    
    int64_t OFF = 0;        // Offset at actual temperature
    int64_t SENS = 0;       // Sensitivity at actual temperature
    
    int64_t Ti = 0;         // Second order temperature component       
    int64_t OFFi = 0;       // Second order offset component            
    int64_t SENSi = 0;      // Second order sens component             
    
    int64_t OFF2 = 0;       // Second order final off 
    int64_t SENS2 = 0;      // Second order final sens
    int64_t TEMP2 = 0;      // Second order final temp
    
    int32_t P = 0;          // Temperature compensated pressure
    
    // Bytes to hold the results from I2C communications with the sensor
    uint8_t HighByte;
    uint8_t MidByte;
    uint8_t LowByte;
    
	uint16_t m_oversampleResolution;
    
    // Some constants used in calculations below
    const uint32_t POW_2_13 = 8192u;            // 2^13
    const uint32_t POW_2_21 = 2097152u;         // 2^21
    const uint64_t POW_2_33 = 8589934592ULL;    // 2^33
    const uint64_t POW_2_37 = 137438953472ULL;  // 2^37
	
	// Methods
	uint8_t GetCRC( uint32_t n_prom[] );
	uint32_t CommandADC( uint8_t commandIn );
	
	int WriteRegisterByte( uint8_t addressIn );
	int WriteDataByte( uint8_t addressIn, uint8_t dataIn );
	int ReadByte( uint8_t addressIn, uint8_t& dataOut );
};