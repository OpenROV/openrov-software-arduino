#pragma once

// Includes
#include <Arduino.h>

// Defines
#define MS5837_WATERTYPE_FRESH  0
#define MS5837_WATERTYPE_SALT   1

#define MS5837_MEAS_PRESSURE    0
#define MS5837_MEAS_TEMPERATURE 1

#define MS5837_ADC_256          0x00	// ADC resolution = 256
#define MS5837_ADC_512          0x02	// ADC resolution = 512
#define MS5837_ADC_1024	        0x04	// ADC resolution = 1024
#define MS5837_ADC_2048        	0x06	// ADC resolution = 2048
#define MS5837_ADC_4096	        0x08	// ADC resolution = 4096
#define MS5837_ADC_8192	        0x0A	// ADC resolution = 8192

class MS5837_30BA
{
public:
   
    // Attributes
    float m_pressure_mbar = 0;
	float m_temperature_c = 0;
	float m_depth_m = 0;
	
	bool m_crcCheckSuccessful = false;
	
	int m_waterType;
	
	uint16_t m_sensorCoeffs[8]; // unsigned 16-bit integer (0-65535)

	 // Methods
	MS5837_30BA( uint8_t resolutionIn = MS5837_ADC_4096 );
	
	int Initialize();
	int GetCalibrationCoefficients();
	int Reset();
	int StartConversion( int measurementTypeIn );
	int Read( int measurementType );
	void CalculateOutputs();
	
	void SetWaterType( int waterTypeIn );

private:

    // Attributes
    uint8_t m_oversampleResolution;
    
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
    uint8_t HighByte = 0;
    uint8_t MidByte = 0;
    uint8_t LowByte = 0;
    
    // Some constants used in calculations below
    const int32_t POW_2_7  = 128u;             // 2^7
    const int32_t POW_2_8  = 256u;             // 2^8
    const int32_t POW_2_13 = 8192u;            // 2^13
    const int32_t POW_2_15 = 32768u;           // 2^15
    const int32_t POW_2_16 = 65536u;           // 2^16
    const int32_t POW_2_21 = 2097152u;         // 2^21
    const int32_t POW_2_23 = 8388608u;         // 2^23
    const int64_t POW_2_33 = 8589934592ULL;    // 2^33
    const int64_t POW_2_37 = 137438953472ULL;  // 2^37
	
	// Methods
	uint8_t CalculateCRC4( uint16_t n_prom[] );
	uint32_t CommandADC( uint8_t commandIn );
	
	int WriteRegisterByte( uint8_t addressIn );
	int WriteDataByte( uint8_t addressIn, uint8_t dataIn );
	int ReadByte( uint8_t addressIn, uint8_t& dataOut );
};