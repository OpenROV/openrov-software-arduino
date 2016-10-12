/***************************************************************************
  This is a library for the BNO055 orientation sensor

  Designed specifically to work with the Adafruit BNO055 Breakout.

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by KTOWN for Adafruit Industries.

  MIT license, all text above must be included in any redistribution
 ***************************************************************************/
 
 // Modified for use in OpenROV's Software

#pragma once
#include <Arduino.h>
#include <I2C.h>

#include "LibBNO055_Definitions.h"
#include "LibBNO055_imumaths.h"

#define BNO055_ADDRESS_A (0x28)
#define BNO055_ADDRESS_B (0x29)
#define BNO055_ID        (0xA0)

class BNO055
{
public:

	BNO055( I2C *i2cInterfaceIn, int32_t sensorIdIn = -1, uint8_t addressIn = BNO055_ADDRESS_A );

	int32_t Initialize();
	int32_t Reset();
	int32_t SetMode( bosch::EOpMode modeIn );
	int32_t SetExternalCrystalUse( bool shouldUseIn );
	int32_t SetPowerMode( bosch::EPowerMode powerModeIn );
	int32_t SetSelectedRegisterPage( byte pageIdIn );
	int32_t SetUpUnitsAndOrientation();
	int32_t VerifyChipId();
	int32_t GetCalibration();
	int32_t GetRevInfo( bosch::TRevisionInfo& revInfoOut );
	int32_t GetVector( bosch::EVectorType vectorTypeIn, imu::Vector<3>& vectorOut );
	int32_t GetQuat( imu::Quaternion& quatOut );
	int32_t GetTemperature( int8_t& temperatureOut );
	int32_t GetPowerOnSelfTestResults();
	int32_t GetSystemStatus();
	int32_t GetSystemError();
	int32_t GetSoftwareVersion();
	int32_t GetBootloaderRev();
	int32_t GetOperatingMode();

	int32_t GetAccelerometerOffsets();
	int32_t GetGyroOffsets();
	int32_t GetMagnetometerOffsets();

	int32_t EnterIMUMode();
	int32_t EnterNDOFMode();

	bool m_isInitialized = false;

	uint8_t m_systemCal;
	uint8_t m_gyroCal;
	uint8_t m_magCal;
	uint8_t m_accelCal;

	uint8_t m_systemStatus;
	uint8_t m_powerOnSelfTestResult;
	uint8_t m_biltSelfTestResult;
	uint8_t m_systemError;

	uint8_t m_softwareVersionMinor;
	uint8_t m_softwareVersionMajor;
	uint8_t m_bootloaderRev;

	uint8_t m_operatingMode;

private:
	int32_t ReadByte( bosch::ERegisterAddress addressIn, uint8_t& dataOut );
	int32_t ReadNBytes( bosch::ERegisterAddress addressIn, uint8_t* dataOut, uint8_t byteCountIn );
	int32_t WriteByte( bosch::ERegisterAddress addressIn, uint8_t dataIn );

	int32_t m_lastRetcode = 0;

	static const unsigned long	k_configToOpDelay_ms	= 30;
	static const unsigned long	k_opToConfigDelay_ms	= 30;
	static const unsigned long	k_resetDelay_ms			= 1000;
	static const unsigned long	k_selfTestDelay_ms		= 1000;

	uint8_t		m_i2cAddress;
	int32_t		m_sensorId;
	bosch::EOpMode		m_mode;

	bosch::TCalibrationOffsets m_accOffsets;
	bosch::TCalibrationOffsets m_gyroOffsets;
	bosch::TCalibrationOffsets m_magOffsets;
	
	I2C *m_pI2C;
};
